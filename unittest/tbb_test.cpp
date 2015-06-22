#include <echo/execution_context/expression.h>
#include <echo/execution_context/tbb/expression_executer.h>
#include <echo/test.h>
#include <numeric>
#include <functional>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::intel_tbb;

TEST_CASE("tbb_vector") {
//   const int N = 16;
//   struct alignas(128) {
//     double data[N];
//   } a_buffer, b_buffer;
//   double(&a)[N] = a_buffer.data;
//   double(&b)[N] = b_buffer.data;
//   std::iota(std::begin(a), std::end(a), 0);
//   auto eval = [&](index_t i) { return b[i] = a[i] * a[i]; };
//   auto expr = make_expression(N, eval);
//   ExpressionExecuter executer;
//
//   CHECK(execution_context::concept::expression_executer<ExpressionExecuter>());
//
//   SECTION("default") {
//     executer(expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("simd") {
//     executer(execution_mode::simd, expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("aligned") {
//     executer(execution_mode::aligned, expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("nontemporal") {
//     executer(execution_mode::nontemporal, expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("parallel1") {
//     executer(execution_mode::parallel_fine, expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("parallel2") {
//     executer(execution_mode::parallel_coarse, expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
//   SECTION("parallel3") {
//     executer(execution_mode::parallel_fine | execution_mode::simd |
//                  execution_mode::nontemporal,
//              expr);
//     REQUIRE(b[0] == a[0] * a[0]);
//     REQUIRE(b[N - 1] == a[N - 1] * a[N - 1]);
//   }
// }
//
// TEST_CASE("tbb_general_matrix") {
//   const int M = 2;
//   const int N = 3;
//   struct alignas(128) {
//     double data[M][N];
//   } a_buffer, b_buffer;
//   double(&a)[M][N] = a_buffer.data;
//   double(&b)[M][N] = b_buffer.data;
//   std::iota(&a[0][0], &a[0][0] + M * N, 0);
//   auto eval = [&](index_t i, index_t, index_t j, index_t) {
//     return b[i][j] = a[i][j] * a[i][j];
//   };
//   auto expr = make_expression(M, N, eval);
//   ExpressionExecuter executer;
//   SECTION("serial") { executer(expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel, expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel_coarse, expr); }
//   REQUIRE(b[0][0] == 0);
//   REQUIRE(b[0][1] == 1);
//   REQUIRE(b[0][2] == 4);
//   REQUIRE(b[1][0] == 9);
//   REQUIRE(b[1][1] == 16);
//   REQUIRE(b[1][2] == 25);
// }
//
// TEST_CASE("tbb_lower_half_matrix") {
//   const int N = 2;
//   double a[N][N];
//   double b[N][N];
//   int count[N][N];
//   std::fill_n(&b[0][0], N * N, 0);
//   std::fill_n(&count[0][0], N * N, 0);
//   std::iota(&a[0][0], &a[0][0] + N * N, 0);
//   auto eval = [&](index_t i, index_t, index_t j, index_t) {
//     ++count[i][j];
//     return b[i][j] = a[i][j] * a[i][j];
//   };
//   auto expr = make_expression<structure::lower_half>(N, N, eval);
//   ExpressionExecuter executer;
//   SECTION("serial") { executer(expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel, expr); }
//   SECTION("parallel2") { executer(execution_mode::parallel_coarse, expr); }
//   REQUIRE(b[0][0] == 0);
//   REQUIRE(b[0][1] == 0);
//   REQUIRE(b[1][0] == 4);
//   REQUIRE(b[1][1] == 9);
//
//   REQUIRE(count[0][0] == 1);
//   REQUIRE(count[0][1] == 0);
//   REQUIRE(count[1][0] == 1);
//   REQUIRE(count[1][1] == 1);
// }
//
// TEST_CASE("tbb_flat_lower_half_matrix") {
//   const int N = 2;
//   double a[N*N] = {1, 2, 3, 4};
//   double b[N*N] = {3, 2, 9, 1};
//   double c[N*N] = {0, 0, 0, 0};
//
//   auto expr = make_expression<structure::lower_half>(N, N,
//     [&](index_t i) {
//       return c[i] = a[i] + b[i];
//     });
//
//   ExpressionExecuter executer;
//   executer(expr);
//
//   CHECK(c[0] == 4);
//   CHECK(c[1] == 4);
//   CHECK(c[2] == 0);
//   CHECK(c[3] == 5);
// }
//
// TEST_CASE("tbb_strict_lower_half_matrix") {
//   const int N = 2;
//   double a[N][N];
//   double b[N][N];
//   int count[N][N];
//   std::fill_n(&b[0][0], N * N, 0);
//   std::fill_n(&count[0][0], N * N, 0);
//   std::iota(&a[0][0], &a[0][0] + N * N, 0);
//   auto eval = [&](index_t i, index_t, index_t j, index_t) {
//     ++count[i][j];
//     return b[i][j] = a[i][j] * a[i][j];
//   };
//   auto expr = make_expression<structure::strict_lower_half>(N, N, eval);
//   ExpressionExecuter executer;
//   SECTION("serial") { executer(expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel, expr); }
//   SECTION("parallel2") { executer(execution_mode::parallel_coarse, expr); }
//   REQUIRE(b[0][0] == 0);
//   REQUIRE(b[0][1] == 0);
//   REQUIRE(b[1][0] == 4);
//   REQUIRE(b[1][1] == 0);
//
//   REQUIRE(count[0][0] == 0);
//   REQUIRE(count[0][1] == 0);
//   REQUIRE(count[1][0] == 1);
//   REQUIRE(count[1][1] == 0);
// }
//
// TEST_CASE("tbb_upper_half_matrix") {
//   const int N = 2;
//   double a[N][N];
//   double b[N][N];
//   int count[N][N];
//   std::fill_n(&b[0][0], N * N, 0);
//   std::fill_n(&count[0][0], N * N, 0);
//   std::iota(&a[0][0], &a[0][0] + N * N, 0);
//   auto eval = [&](index_t i, index_t, index_t j, index_t) {
//     ++count[i][j];
//     return b[i][j] = a[i][j] * a[i][j];
//   };
//   auto expr = make_expression<structure::upper_half>(N, N, eval);
//   ExpressionExecuter executer;
//   SECTION("serial") { executer(expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel, expr); }
//   SECTION("parallel2") { executer(execution_mode::parallel_coarse, expr); }
//   REQUIRE(b[0][0] == 0);
//   REQUIRE(b[0][1] == 1);
//   REQUIRE(b[1][0] == 0);
//   REQUIRE(b[1][1] == 9);
//
//   REQUIRE(count[0][0] == 1);
//   REQUIRE(count[0][1] == 1);
//   REQUIRE(count[1][0] == 0);
//   REQUIRE(count[1][1] == 1);
// }
//
// TEST_CASE("tbb_strict_upper_half_matrix") {
//   const int N = 2;
//   double a[N][N];
//   double b[N][N];
//   int count[N][N];
//   std::fill_n(&b[0][0], N * N, 0);
//   std::fill_n(&count[0][0], N * N, 0);
//   std::iota(&a[0][0], &a[0][0] + N * N, 0);
//   auto eval = [&](index_t i, index_t, index_t j, index_t) {
//     ++count[i][j];
//     return b[i][j] = a[i][j] * a[i][j];
//   };
//   auto expr = make_expression<structure::strict_upper_half>(N, N, eval);
//   ExpressionExecuter executer;
//   SECTION("serial") { executer(expr); }
//   SECTION("parallel1") { executer(execution_mode::parallel, expr); }
//   SECTION("parallel2") { executer(execution_mode::parallel_coarse, expr); }
//   REQUIRE(b[0][0] == 0);
//   REQUIRE(b[0][1] == 1);
//   REQUIRE(b[1][0] == 0);
//   REQUIRE(b[1][1] == 0);
//
//   REQUIRE(count[0][0] == 0);
//   REQUIRE(count[0][1] == 1);
//   REQUIRE(count[1][0] == 0);
//   REQUIRE(count[1][1] == 0);
// }
//
// TEST_CASE("plus reduction") {
//   const int N = 5;
//   double a[N];
//   std::iota(std::begin(a), std::end(a), 0);
//   auto expr = make_reduction_expression(N, [&](index_t i) { return a[i]; },
//                                         std::plus<double>(), 0.0);
//   double result = 0;
//   ExpressionExecuter executer;
//   SECTION("serial") { result = executer(expr); }
//   SECTION("parallel1") { result = executer(execution_mode::parallel, expr); }
//   SECTION("serial") {
//     result = executer(execution_mode::parallel_coarse, expr);
//   }
//   CHECK(result == 10);
// }
//
// TEST_CASE("multiplies reduction") {
//   const int N = 5;
//   double a[N];
//   std::iota(std::begin(a), std::end(a), 1);
//   auto expr = make_reduction_expression(N, [&](index_t i) { return a[i]; },
//                                         std::multiplies<double>(), 1.0);
//   double result = 0;
//   ExpressionExecuter executer;
//   SECTION("serial") { result = executer(expr); }
//   SECTION("parallel1") { result = executer(execution_mode::parallel, expr); }
//   SECTION("serial") {
//     result = executer(execution_mode::parallel_coarse, expr);
//   }
//   CHECK(result == 120);
}

TEST_CASE("matrix plus reduction") {
  // const int M = 2, N = 3;
  // double a[M][N];
  // std::iota(&a[0][0], &a[0][0] + M * N, 0);
  // auto expr = make_reduction_expression(
  //     make_k_shape(M, N), [&](index_t i, index_t, index_t j, index_t) {
  //       return a[i][j];
  //     }, std::plus<double>(), 0.0);
  // ExpressionExecuter executer;
  // double result = 0;
  // SECTION("serial") { result = executer(expr); }
  // SECTION("parallel1") { result = executer(execution_mode::parallel, expr); }
  // SECTION("parallel1") {
  //   result = executer(execution_mode::parallel_coarse, expr);
  // }
  // CHECK(result == 15);
}
