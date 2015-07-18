#include <echo/execution_context/mkl/lapack_executer.h>
#include <echo/execution_context/lapack_executer_concept.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::execution_context;
using namespace echo::execution_context::intel_mkl;

static LapackExecuter lapack_executer;

TEST_CASE("mkl_lapack_executer") {
  CHECK(execution_context::concept::lapack_executer<LapackExecuter>());
  CHECK(!execution_context::concept::lapack_executer<int>());
}

