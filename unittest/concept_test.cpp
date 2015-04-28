#include <echo/execution_context/test/expression.h>
#include <echo/execution_context/concept.h>
#include <echo/test.h>
#include <functional>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::test;

TEST_CASE("concept") {
  auto eval1 = [](index_t x) -> double { return x * x; };

  REQUIRE(execution_context::concept::flat_evaluator<decltype(eval1)>());

  auto expr1 = make_expression(10, [](index_t x) -> double { return x * x; });
  REQUIRE(execution_context::concept::flat_expression<decltype(expr1)>());
  REQUIRE(
      !execution_context::concept::shaped_matrix_expression<decltype(expr1)>());
  REQUIRE(execution_context::concept::expression<decltype(expr1)>());

  auto expr2 = make_expression(
      3, 7_index, [](index_t i, index_t, index_t j, index_t) { return 3.7; });
  REQUIRE(!execution_context::concept::flat_expression<decltype(expr2)>());
  REQUIRE(
      execution_context::concept::shaped_matrix_expression<decltype(expr2)>());
  REQUIRE(execution_context::concept::expression<decltype(expr2)>());

  auto expr3 = make_reduction_expression(
      5_index, [](index_t i) { return 22.0; }, std::plus<double>(), 0.0);
  REQUIRE(
      execution_context::concept::flat_reduction_expression<decltype(expr3)>());
  REQUIRE(!execution_context::concept::flat_expression<decltype(expr3)>());
  // REQUIRE(!execution_context::concept::expression<decltype(expr3)>());

  auto expr4 = make_reduction_expression(
      make_k_shape(2_index, 3_index),
      [](index_t i, index_t ni, index_t j, index_t nj) { return 44.7; },
      std::plus<double>(), 0.0);
  REQUIRE(execution_context::concept::shaped_reduction_expression<decltype(
      expr4)>());
  REQUIRE(!execution_context::concept::flat_reduction_expression<decltype(
      expr4)>());
  REQUIRE(!execution_context::concept::flat_expression<decltype(expr4)>());
  // REQUIRE(!execution_context::concept::expression<decltype(expr4)>());
}
