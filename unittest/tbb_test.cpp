#include <echo/execution_context/test/expression.h>
#include <echo/execution_context/tbb/expression_executer.h>
#include <echo/test.h>
#include <numeric>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::test;
using namespace echo::execution_context::intel_tbb;

TEST_CASE("square_vector") {
  const int N = 16;
  double a[N], b[N];
  std::iota(std::begin(a), std::end(a), 0);
  auto eval = [&](index_t i) { return b[i] = a[i] * a[i]; };
  auto expr = make_expression(N, eval);
  ExpressionExecuter executer;
  SECTION("default") {
    executer(expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("simd") {
    executer(execution_mode::simd, expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("aligned") {
    executer(execution_mode::aligned, expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("nontemporal") {
    executer(execution_mode::nontemporal, expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("parallel1") {
    executer(execution_mode::parallel_fine, expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("parallel2") {
    executer(execution_mode::parallel_coarse, expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
  SECTION("parallel3") {
    executer(execution_mode::parallel_fine | execution_mode::simd |
                 execution_mode::nontemporal,
             expr);
    REQUIRE(b[0] == a[0] * a[0]);
    REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
  }
}
