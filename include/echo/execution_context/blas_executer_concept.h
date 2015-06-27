#pragma once

#define DETAIL_NS detail_blas_execution_concept

#include <echo/execution_context/matrix_utility.h>
#include <echo/concept.h>

namespace echo {
namespace execution_context {
namespace concept {

namespace DETAIL_NS {
template <class Scalar>
struct BlasExecuter : Concept {
  template <class T>
  auto require(T&& executer) -> list<
      same<Scalar, decltype(executer.dot(
                       index_t(), std::declval<const Scalar*>(), index_t(),
                       std::declval<const Scalar*>(), index_t()))>(),
      valid<decltype(executer.gemv(
          matrix_operation_t(), index_t(), index_t(), std::declval<Scalar>(),
          std::declval<const Scalar*>(), index_t(),
          std::declval<const Scalar*>(), index_t(), std::declval<Scalar>(),
          std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.trsv(structure::uplo_t(), matrix_operation_t(),
                                   matrix_diagonal_fill_t(), index_t(),
                                   std::declval<const Scalar*>(), index_t(),
                                   std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.symv(
          structure::uplo_t(), index_t(), std::declval<Scalar>(),
          std::declval<const Scalar*>(), index_t(),
          std::declval<const Scalar*>(), index_t(), std::declval<Scalar>(),
          std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.syr(structure::uplo_t(), index_t(),
                                  std::declval<Scalar>(),
                                  std::declval<const Scalar*>(), index_t(),
                                  std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.syr2(structure::uplo_t(), index_t(),
                                   std::declval<Scalar>(),
                                   std::declval<const Scalar*>(), index_t(),
                                   std::declval<const Scalar*>(), index_t(),
                                   std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.gemm(
          matrix_operation_t(), matrix_operation_t(), index_t(), index_t(),
          index_t(), std::declval<Scalar>(), std::declval<const Scalar*>(),
          index_t(), std::declval<const Scalar*>(), index_t(),
          std::declval<Scalar>(), std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.trsm(
          matrix_side_t(), structure::uplo_t(), matrix_operation_t(),
          matrix_diagonal_fill_t(), index_t(), index_t(),
          std::declval<Scalar>(), std::declval<const Scalar*>(), index_t(),
          std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.syrk(
          structure::uplo_t(), matrix_operation_t(), index_t(), index_t(),
          std::declval<Scalar>(), std::declval<const Scalar*>(), index_t(),
          std::declval<Scalar>(), std::declval<Scalar*>(), index_t()))>(),
      valid<decltype(executer.symm(
          matrix_side_t(), structure::uplo_t(), index_t(), index_t(),
          std::declval<Scalar>(), std::declval<const Scalar*>(), index_t(),
          std::declval<const Scalar*>(), index_t(), std::declval<Scalar>(),
          std::declval<Scalar*>(), index_t()))>()>;
};
}

template <class T>
constexpr bool blas_executer() {
  return models<DETAIL_NS::BlasExecuter<float>, T>() &&
         models<DETAIL_NS::BlasExecuter<double>, T>();
}
}
}
}

#undef DETAIL_NS
