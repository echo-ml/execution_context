#pragma once
#include <echo/tbb_expression_executer.h>
#include <echo/mkl_blas_executer.h>
#include <echo/mkl_lapack_executer.h>
#include <echo/execution_context.h>

namespace echo {
namespace execution_context {
namespace intel {

struct ExecutionContext : AllocationBackend<std::allocator>,
                          intel_mkl::BlasExecuter,
                          intel_mkl::LapackExecuter,
                          intel_tbb::ExpressionExecuter {};
}
}
}
