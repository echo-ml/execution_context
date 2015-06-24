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

BENCHMARK_SET("tbb_expression_execution", NumTrials(1),
              LinearRange(1, 100'000, 10'000), Units<std::chrono::microseconds>()) 
              {
  auto N = touchstone::n();
  auto rng = touchstone::random_number_generator();
  std::uniform_real_distribution<double> dist(-1, 1);
  std::vector<double> x(N), y(N);
  for (auto&& xi : x) xi = dist(rng);
  auto expr =
      make_expression(N, [&](int i) { return y[i] = std::sqrt(x[i]); });
  ExpressionExecuter executer;
  BENCHMARK("base") {
    for (int i = 0; i < N; ++i) y[i] = std::sqrt(x[i]);
  }
  BENCHMARK("execution") { executer(expr); }
  BENCHMARK("parallel") { executer(execution_mode::parallel_fine, expr); }
  BENCHMARK("nontemporal-parallel") {
    executer(execution_mode::parallel_fine | execution_mode::nontemporal |
                 execution_mode::simd,
             expr);
  }

  Eigen::VectorXd v(N), w(N);
  for (int i=0; i<N; ++i)
    v(i) = dist(rng);

  BENCHMARK("eigen") {
    w = v.cwiseSqrt();
  }
}

BENCHMARK_SET("square", NumTrials(1),
              LinearRange(1, 1'000'000, 100'000), Units<std::chrono::microseconds>()) 
{
  auto N = touchstone::n();
  auto rng = touchstone::random_number_generator();
  std::uniform_real_distribution<double> dist(-1, 1);
  std::vector<double> x(N), y(N);
  for (auto&& xi : x) xi = dist(rng);
  auto expr =
      make_expression(N, [&](int i) { return y[i] = x[i]*x[i]; });
  ExpressionExecuter executer;
  BENCHMARK("base") {
    for (int i = 0; i < N; ++i) y[i] = x[i]*x[i];
  }
  BENCHMARK("execution") { executer(expr); }
  BENCHMARK("nontemporal") {
    executer(execution_mode::nontemporal |
                 execution_mode::simd,
             expr);
  }

  Eigen::VectorXd v(N), w(N);
  for (int i=0; i<N; ++i)
    v(i) = dist(rng);
  BENCHMARK("eigen") {
    w = v.array()*v.array();
  }
}
