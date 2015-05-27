#pragma once

#include <echo/execution_context/matrix_utility.h>
#include <echo/concept2.h>

namespace echo {
namespace execution_context {
namespace concept {

namespace detail {
namespace blas_executer_concept {

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
}

template <class T>
constexpr bool blas_executer() {
  return models<detail::blas_executer_concept::BlasExecuter<float>, T>() &&
         models<detail::blas_executer_concept::BlasExecuter<double>, T>();
}
}
}
}
