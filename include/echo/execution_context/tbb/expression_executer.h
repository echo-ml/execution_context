#pragma once

#include <echo/execution_context/tbb/for.h>
#include <echo/execution_context/concept.h>
#include <echo/execution_context/option.h>
#include <tbb/tbb.h>

namespace echo {
namespace execution_context {
namespace intel_tbb {

class ExpressionExecuter {
  static constexpr auto kDefaultOptions =
      execution_mode::simd | execution_mode::serial |
      execution_mode::nontemporal | execution_mode::nonaligned;

  static constexpr std::size_t kCoarseGrainularity = 100'000;

 public:
  template <
      class Expression,
      CONCEPT_REQUIRES(execution_context::concept::expression<Expression>())>
  auto operator()(const Expression& expression) const {
    return this->operator()(kDefaultOptions, expression);
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                       execution_context::concept::expression<Expression>())>
  auto operator()(Options options, const Expression& expression) const {
    return execute(kDefaultOptions | options, expression);
  }

 private:
  ////////////
  // vector //
  ////////////

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::vector_expression<Expression>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::serial)>
  void execute(Options options, const Expression& expression) const {
    detail::for_(options, 0, get_num_elements(expression),
                 expression.evaluator());
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::vector_expression<Expression>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_fine)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(
        tbb::blocked_range<index_t>(0, get_num_elements(expression)),
        [&](tbb::blocked_range<index_t> range) {
          detail::for_(options, range.begin(), range.end(),
                       expression.evaluator());
        });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::vector_expression<Expression>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_coarse)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(tbb::blocked_range<index_t>(
                          0, get_num_elements(expression), kCoarseGrainularity),
                      [&](tbb::blocked_range<index_t> range) {
                        detail::for_(options, range.begin(), range.end(),
                                     expression.evaluator());
                      });
  }

  ////////////////////
  // general-matrix //
  ////////////////////

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_fine &&
                execution_context::concept::matrix_expression<Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(
        tbb::blocked_range2d<index_t, index_t>(0, get_extent<0>(expression), 0,
                                               get_extent<1>(expression)),
        [&](tbb::blocked_range2d<index_t, index_t> range) {
          execute(options, range.rows().begin(), range.rows().end(),
                  range.cols().begin(), range.cols().end(), expression);
        });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_coarse &&
                execution_context::concept::matrix_expression<Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    auto num_rows = get_extent<0>(expression);
    auto num_columns = get_extent<1>(expression);
    auto column_grainularity =
        kCoarseGrainularity / num_rows + (kCoarseGrainularity % num_rows > 0);
    tbb::parallel_for(tbb::blocked_range2d<index_t, index_t>(
                          0, num_rows, kCoarseGrainularity, 0, num_columns,
                          column_grainularity),
                      [&](tbb::blocked_range2d<index_t, index_t> range) {
                        execute(options, range.rows().begin(),
                                range.rows().end(), range.cols().begin(),
                                range.cols().end(), expression);
                      });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::serial &&
                execution_context::concept::matrix_expression<Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    execute(options, 0, get_extent<0>(expression), 0, get_extent<1>(expression),
            expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::matrix_expression<Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::general>::value)>
  void execute(Options options, index_t row_first, index_t row_last,
               index_t column_first, index_t column_last,
               const Expression& expression) const {
    auto evaluator = expression.evaluator();
    for (index_t j = column_first; j < column_last; ++j)
      detail::for_(options, row_first, row_last, [&](index_t i) {
        return evaluator(i, get_extent<0>(expression), j,
                         get_extent<1>(expression));
      });
  }

  /////////////////
  // half-matrix //
  /////////////////

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::half_matrix_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_fine)>
  void execute(Options options, const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto half_n = n / 2 + (n % 2);
    tbb::parallel_for(tbb::blocked_range<index_t>(0, half_n),
                      [&](tbb::blocked_range<index_t> range) {
                        execute_half_ranges(options, range.begin(), range.end(),
                                            expression);
                      });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::half_matrix_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_coarse)>
  void execute(Options options, const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto half_n = n / 2 + (n % 2);
    tbb::parallel_for(
        tbb::blocked_range<index_t>(0, half_n, kCoarseGrainularity),
        [&](tbb::blocked_range<index_t> range) {
          execute_half_ranges(options, range.begin(), range.end(), expression);
        });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::half_matrix_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::serial)>
  void execute(Options options, const Expression& expression) const {
    auto n = get_extent<0>(expression);
    execute(options, 0, n, expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             execution_context::concept::half_matrix_expression<
                                 Expression>())>
  void execute_half_ranges(Options options, index_t first, index_t last,
                           const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto first1 = first;
    auto last1 = last;
    auto first2 = n - last1;
    auto last2 = n - first1;
    if (first2 <= last1) {
      last1 = last2;
      first2 = 0;
      last2 = 0;
    }

    execute(options, first1, last1, expression);
    execute(options, first2, last2, expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             execution_context::concept::half_matrix_expression<
                                 Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::lower_half>::value)>
  void execute(Options options, index_t first, index_t last,
               const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto evaluator = expression.evaluator();
    for (index_t j = first; j < last; ++j)
      detail::for_(options, j, n,
                   [&](index_t i) { return evaluator(i, n, j, n); });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             execution_context::concept::half_matrix_expression<
                                 Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::strict_lower_half>::value)>
  void execute(Options options, index_t first, index_t last,
               const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto evaluator = expression.evaluator();
    for (index_t j = first; j < last; ++j)
      detail::for_(options, j + 1, n,
                   [&](index_t i) { return evaluator(i, n, j, n); });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             execution_context::concept::half_matrix_expression<
                                 Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::upper_half>::value)>
  void execute(Options options, index_t first, index_t last,
               const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto evaluator = expression.evaluator();
    for (index_t j = first; j < last; ++j)
      detail::for_(options, 0, j + 1,
                   [&](index_t i) { return evaluator(i, n, j, n); });
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             execution_context::concept::half_matrix_expression<
                                 Expression>()),
            CONCEPT_REQUIRES(
                std::is_convertible<expression_traits::structure<Expression>,
                                    structure::strict_upper_half>::value)>
  void execute(Options options, index_t first, index_t last,
               const Expression& expression) const {
    auto n = get_extent<0>(expression);
    auto evaluator = expression.evaluator();
    for (index_t j = first; j < last; ++j)
      detail::for_(options, 0, j,
                   [&](index_t i) { return evaluator(i, n, j, n); });
  }

  //////////////////////////
  // reduction-expression //
  //////////////////////////

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::reduction_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_fine)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    return tbb::parallel_reduce(
        tbb::blocked_range<index_t>(0, get_num_elements(expression)),
        expression.identity(),
        [&](tbb::blocked_range<index_t> range, Scalar init) {
          return execute(options, range.begin(), range.end(), init, expression);
        },
        expression.reducer());
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::reduction_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_coarse)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    return tbb::parallel_reduce(
        tbb::blocked_range<index_t>(0, get_num_elements(expression),
                                    kCoarseGrainularity),
        expression.identity(),
        [&](tbb::blocked_range<index_t> range, Scalar init) {
          return execute(options, range.begin(), range.end(), init, expression);
        },
        expression.reducer());
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::reduction_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::serial)>
  auto execute(Options options, const Expression& expression) const {
    return execute(options, 0, get_num_elements(expression),
                   expression.identity(), expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::reduction_expression<Expression>())>
  auto execute(Options options, index_t first, index_t last,
               uncvref_t<decltype(std::declval<Expression>().identity())> init,
               const Expression& expression) const {
    auto result = init;
    auto mapper = expression.mapper();
    auto reducer = expression.reducer();
    detail::for_(options, first, last, [&](index_t i) {
      return result = reducer(result, mapper(i));
    });
    return result;
  }
};
}
}
}
