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

 private:
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

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                get_option<execution_mode::parallel_t, Options>() ==
                    execution_mode::parallel_fine &&
                execution_context::concept::matrix_expression<Expression>())>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(
        tbb::blocked_range2d<index_t, index_t>(0, get_extent<0>(expression), 0,
                                               get_extent<0>(expression)),
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
                execution_context::concept::matrix_expression<Expression>())>
  void execute(Options options, const Expression& expression) const {
    auto num_rows = get_extent<0>(expression);
    auto num_columns = get_extent<1>(expression);
    tbb::parallel_for(tbb::blocked_range2d<index_t, index_t>(
                          0, num_rows, kCoarseGrainularity, 0, num_columns,
                          kCoarseGrainularity / num_rows),
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
                execution_context::concept::matrix_expression<Expression>())>
  void execute(Options options, const Expression& expression) const {
    execute(options, 0, get_extent<0>(expression), 0, get_extent<1>(expression),
            expression);
  }

  template <class Options, class Expression,
            CONCEPT_REQUIRES(
                option::concept::option_list<Options>() &&
                execution_context::concept::matrix_expression<Expression>())>
  void execute(Options options, index_t row_first, index_t row_last,
               index_t column_first, index_t column_last,
               const Expression& expression) const {
    auto evaluator = expression.evaluator();
    for (index_t j = column_first; j < column_last; ++j)
      detail::for_(options, row_first, row_last,
                   [&](index_t i) { evaluator(i, j); });
  }
};
}
}
}
