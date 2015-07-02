#pragma once

#include <echo/execution_context/tbb/for.h>
#include <echo/execution_context/tbb/blocked_range.h>
#include <echo/execution_context/concept.h>
#include <echo/execution_context/option.h>
#include <echo/execution_context/expression.h>
#include <tbb/tbb.h>

namespace echo {
namespace execution_context {
namespace intel_tbb {

//------------------------------------------------------------------------------
// ExpressionExecuter
//------------------------------------------------------------------------------
class ExpressionExecuter {
  static constexpr auto kDefaultOptions =
      execution_mode::nosimd | execution_mode::serial |
      execution_mode::temporal | execution_mode::nonaligned |
      execution_mode::suggest_inline;

  static constexpr std::size_t kCoarseGrainularity = 100'000;

 public:
  template <
      class Expression,
      CONCEPT_REQUIRES(execution_context::concept::expression<Expression>())>
  void operator()(const Expression& expression) const {
    this->operator()(kDefaultOptions, expression);
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                       execution_context::concept::expression<Expression>())>
  void operator()(Options options, const Expression& expression) const {
    execute(kDefaultOptions | options, expression);
  }

  template <class Expression,
            CONCEPT_REQUIRES(
                execution_context::concept::reduction_expression<Expression>())>
  auto operator()(const Expression& expression) const {
    return this->operator()(kDefaultOptions, expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::reduction_expression<Expression>())>
  auto operator()(Options options, const Expression& expression) const {
    return execute(kDefaultOptions | options, expression);
  }

 private:
  // flat_evaluator
  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::flat_evaluator_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::serial),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    detail::for_(options, 0, get_num_elements(expression),
                 expression.evaluator());
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::flat_evaluator_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_fine),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(
        tbb::blocked_range<index_t>(0, get_num_elements(expression)),
        [&](tbb::blocked_range<index_t> range) {
          detail::for_(options, range.begin(), range.end(),
                       expression.evaluator());
        });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::flat_evaluator_expression<Expression>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_coarse),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(tbb::blocked_range<index_t>(
                          0, get_num_elements(expression), kCoarseGrainularity),
                      [&](tbb::blocked_range<index_t> range) {
                        detail::for_(options, range.begin(), range.end(),
                                     expression.evaluator());
                      });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::flat_evaluator_expression<Expression>()),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::square>::value)>
  void execute(Options options, const Expression& expression) const {
    auto evaluator = expression.evaluator();
    auto shaped_evaluator = [=](index_t i, index_t m, index_t j, index_t n) {
      return evaluator(i + n * j);
    };
    auto shaped_expression =
        make_expression<expression_traits::structure<Expression>>(
            expression.dimensionality(), shaped_evaluator);
    this->execute(options, shaped_expression);
  }

  // general-shaped-expression
  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_fine &&
          execution_context::concept::k_evaluator_expression<Expression>()),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    const int NumDimensions = expression_traits::num_dimensions<Expression>();
    tbb::parallel_for(
        make_blocked_range(expression.dimensionality()),
        [&](const KBlockedRange<NumDimensions, index_t>& blocked_range) {
          execute(options, std::make_index_sequence<NumDimensions>(),
                  blocked_range, expression.evaluator());
        });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::parallel_coarse &&
          execution_context::concept::k_evaluator_expression<Expression>()),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    const int NumDimensions = expression_traits::num_dimensions<Expression>();
    tbb::parallel_for(
        make_blocked_range(expression.dimensionality(), kCoarseGrainularity),
        [&](const KBlockedRange<NumDimensions, index_t>& blocked_range) {
          execute(options, std::make_index_sequence<NumDimensions>(),
                  blocked_range, expression.evaluator());
        });
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          get_option<execution_mode::parallel_t, Options>() ==
              execution_mode::serial &&
          execution_context::concept::k_evaluator_expression<Expression>()),
      CONCEPT_REQUIRES(std::is_convertible<
          expression_traits::structure<Expression>, structure::general>::value)>
  void execute(Options options, const Expression& expression) const {
    const int NumDimensions = expression_traits::num_dimensions<Expression>();
    execute(options, std::make_index_sequence<NumDimensions>(),
            make_blocked_range(expression.dimensionality()),
            expression.evaluator());
  }

  template <class Options, std::size_t IndexFirst, std::size_t... IndexesRest,
            int N, class Evaluator,
            CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                             sizeof...(IndexesRest) > 0 &&
                             execution_context::concept::k_evaluator<
                                 1 + sizeof...(IndexesRest), Evaluator>())>
  void execute(Options options, std::index_sequence<IndexFirst, IndexesRest...>,
               const KBlockedRange<N, index_t>& blocked_range,
               const Evaluator& evaluator) const {
    auto projection = project<N - 1 - IndexFirst>(blocked_range);
    auto size_last = projection.size();
    for (index_t index_last = projection.begin();
         index_last != projection.end(); ++index_last) {
      execute(options, std::index_sequence<IndexesRest...>(), blocked_range,
              [&](auto... indexes_rest) {
                return evaluator(indexes_rest..., index_last, size_last);
              });
    }
  }

  template <
      class Options, int N, class Evaluator,
      CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                       execution_context::concept::k_evaluator<1, Evaluator>())>
  void execute(Options options, std::index_sequence<N - 1>,
               const KBlockedRange<N, index_t>& blocked_range,
               const Evaluator& evaluator) const {
    auto projection = project<0>(blocked_range);
    auto size = projection.size();
    detail::for_(options, projection.begin(), projection.end(),
                 [&](index_t i) { return evaluator(i, size); });
  }

  // half-matrix
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

  // flat-reduction-expression
  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::flat_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_fine)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    return tbb::parallel_reduce(
        tbb::blocked_range<index_t>(0, get_num_elements(expression)),
        expression.identity(),
        [&](tbb::blocked_range<index_t> range, Scalar init) {
          return map_reduce(options, range.begin(), range.end(), init,
                            expression);
        },
        expression.reducer());
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::flat_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_coarse)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    return tbb::parallel_reduce(
        tbb::blocked_range<index_t>(0, get_num_elements(expression),
                                    kCoarseGrainularity),
        expression.identity(),
        [&](tbb::blocked_range<index_t> range, Scalar init) {
          return map_reduce(options, range.begin(), range.end(), init,
                            expression);
        },
        expression.reducer());
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::flat_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::serial)>
  auto execute(Options options, const Expression& expression) const {
    return map_reduce(options, 0, get_num_elements(expression),
                      expression.identity(), expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::flat_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>())>
  auto map_reduce(
      Options options, index_t first, index_t last,
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

  // shaped-reduction-expression
  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::k_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_fine)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    constexpr int NumDimensions =
        expression_traits::num_dimensions<Expression>();
    return tbb::parallel_reduce(
        make_blocked_range(expression.dimensionality()), expression.identity(),
        [&](const KBlockedRange<NumDimensions, index_t>& blocked_range,
            Scalar init) {
          return map_reduce(options, blocked_range, init, expression);
        },
        expression.reducer());
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::k_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_coarse)>
  auto execute(Options options, const Expression& expression) const {
    using Scalar = uncvref_t<decltype(std::declval<Expression>().identity())>;
    constexpr int NumDimensions =
        expression_traits::num_dimensions<Expression>();
    return tbb::parallel_reduce(
        make_blocked_range(expression.dimensionality(), kCoarseGrainularity),
        expression.identity(),
        [&](const KBlockedRange<NumDimensions, index_t>& blocked_range,
            Scalar init) {
          return map_reduce(options, blocked_range, init, expression);
        },
        expression.reducer());
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::k_evaluator_reduction_expression<
                    Expression>() &&
                structure::concept::general<
                    reduction_expression_traits::structure<Expression>>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::serial)>
  auto execute(Options options, const Expression& expression) const {
    return map_reduce(options, make_blocked_range(expression.dimensionality()),
                      expression.identity(), expression);
  }

  template <
      class Options, class Scalar, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::k_evaluator_reduction_expression<
              Expression>() &&
          structure::concept::general<
              reduction_expression_traits::structure<Expression>>()
          // check is broken with intel compiler
          // && std::is_same<Scalar, reduction_expression_traits::value_type<
          //                          Expression>>::value
          )>
  auto map_reduce(Options options,
                  const KBlockedRange<
                      reduction_expression_traits::num_dimensions<Expression>(),
                      index_t>& blocked_range,
                  Scalar init, const Expression& expression) const {
    auto result = init;
    map_reduce(options,
               std::make_index_sequence<
                   reduction_expression_traits::num_dimensions<Expression>()>(),
               blocked_range, expression.mapper(), expression.reducer(),
               result);
    return result;
  }

  template <
      std::size_t IndexFirst, std::size_t... IndexesRest, int NumDimensions,
      class Options, class Mapper, class Reducer, class Scalar,
      CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                       sizeof...(IndexesRest) > 0 &&
                       execution_context::concept::k_evaluator<
                           1 + sizeof...(IndexesRest), Mapper>() &&
                       execution_context::concept::reducer<Scalar, Reducer>())>
  void map_reduce(Options options,
                  std::index_sequence<IndexFirst, IndexesRest...>,
                  const KBlockedRange<NumDimensions, index_t>& blocked_range,
                  const Mapper& mapper, const Reducer& reducer,
                  Scalar& result) const {
    auto projection = project<NumDimensions - 1 - IndexFirst>(blocked_range);
    const index_t size_last = projection.size();
    for (index_t index_last = projection.begin();
         index_last != projection.end(); ++index_last) {
      map_reduce(options, std::index_sequence<IndexesRest...>(),
                 blocked_range, [&](auto... indexes_rest) {
                   return mapper(indexes_rest..., index_last, size_last);
                 }, reducer, result);
    }
  }

  template <
      int NumDimensions, class Options, class Mapper, class Reducer,
      class Scalar,
      CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                       execution_context::concept::k_evaluator<1, Mapper>() &&
                       execution_context::concept::reducer<Scalar, Reducer>())>
  void map_reduce(Options options, std::index_sequence<NumDimensions - 1>,
                  const KBlockedRange<NumDimensions, index_t>& blocked_range,
                  const Mapper& mapper, const Reducer& reducer,
                  Scalar& result) const {
    auto projection = project<0>(blocked_range);
    const index_t size = projection.size();
    detail::for_(options, projection.begin(), projection.end(),
                 [&](index_t index) { return result += mapper(index, size); });
  }
};
}
}
}
