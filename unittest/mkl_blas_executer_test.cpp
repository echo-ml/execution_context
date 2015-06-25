#include <echo/execution_context/mkl/blas_executer.h>
#include <echo/execution_context/blas_executer_concept.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::intel_mkl;

static BlasExecuter blas_executer;

TEST_CASE("mkl_blas_executer") {
  CHECK(execution_context::concept::blas_executer<BlasExecuter>());
}
