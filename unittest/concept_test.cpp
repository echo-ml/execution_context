#include <echo/execution_context/test/expression.h>
#include <echo/execution_context/concept.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::test;

TEST_CASE("concept") {
  auto expr1 = make_expression(10, [](index_t x) -> double { return x * x; });
  REQUIRE(execution_context::concept::vector_expression<decltype(expr1)>());
  REQUIRE(!execution_context::concept::matrix_expression<decltype(expr1)>());
  REQUIRE(execution_context::concept::expression<decltype(expr1)>());

  auto expr2 = make_expression(
      3, 7_index, [](index_t i, index_t, index_t j, index_t) { return 3.7; });
  REQUIRE(!execution_context::concept::vector_expression<decltype(expr2)>());
  REQUIRE(execution_context::concept::matrix_expression<decltype(expr2)>());
  REQUIRE(execution_context::concept::expression<decltype(expr2)>());
}
