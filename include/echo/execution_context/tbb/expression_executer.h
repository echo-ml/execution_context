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
                execution_context::concept::vector_expression<Expression>()),
            CONCEPT_REQUIRES(get_option<execution_mode::parallel_t,
                                        Options>() == execution_mode::serial)>
  void execute(Options options, const Expression& expression) const {
    detail::for_(options, 0, get_num_elements(expression),
                 expression.evaluator());
  }

  template <
      class Options, class Expression,
      CONCEPT_REQUIRES(
          option::concept::option_list<Options>() &&
          execution_context::concept::vector_expression<Expression>()),
      CONCEPT_REQUIRES(get_option<execution_mode::parallel_t, Options>() ==
                       execution_mode::parallel_fine)>
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
          execution_context::concept::vector_expression<Expression>()),
      CONCEPT_REQUIRES(get_option<execution_mode::parallel_t, Options>() ==
                       execution_mode::parallel_coarse)>
  void execute(Options options, const Expression& expression) const {
    tbb::parallel_for(tbb::blocked_range<index_t>(
                          0, get_num_elements(expression), kCoarseGrainularity),
                      [&](tbb::blocked_range<index_t> range) {
                        detail::for_(options, range.begin(), range.end(),
                                     expression.evaluator());
                      });
  }
};
}
}
}
