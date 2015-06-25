#include <echo/execution_context/expression.h>
#include <echo/execution_context/tbb/expression_executer.h>
#include <touchstone/touchstone.h>
#include <numeric>
#include <random>
#include <cmath>
#include <Eigen/Dense>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::intel_tbb;

BENCHMARK_SET("tbb_half_matrix", NumTrials(1), LinearRange(1, 10'000, 1'000),
              Units<std::chrono::microseconds>()) {
  auto N = touchstone::n();
  auto rng = touchstone::random_number_generator();
  std::uniform_real_distribution<double> dist(0, 1);
  std::vector<double> x(N * N), y(N * N);
  for (auto&& xi : x) xi = dist(rng);
  auto expr1 = make_expression<structure::lower_half>(
      N, N, [&](index_t i, index_t n, index_t j, index_t) {
        return y[i + n * j] = std::sqrt(x[i + n * j]);
      });
  auto expr2 =
      make_expression(N, N, [&](index_t i, index_t n, index_t j, index_t) {
        return y[i + n * j] = std::sqrt(x[i + n * j]);
      });
  ExpressionExecuter executer;
  BENCHMARK("half_expr") { 
    // executer(execution_mode::simd, expr1); 
  }
  BENCHMARK("half_parallel_expr") {
    // executer(execution_mode::simd | execution_mode::parallel, expr1);
  }
  BENCHMARK("full_expr") { 
    // executer(execution_mode::simd, expr2); 
  }
}

