#pragma once

#include <echo/execution_context/concept.h>
#include <echo/execution_context/blas_executer_concept.h>
#include <echo/execution_context/lapack_executer_concept.h>
#include <echo/execution_context/expression_traits.h>
#include <echo/execution_context/option.h>
#include <echo/execution_context/matrix_utility.h>
#include <echo/execution_context/allocation_backend.h>
#include <echo/execution_context/execution_context.h>

namespace echo {
using echo::execution_context::make_allocator;
using echo::execution_context::make_aligned_allocator;
}
