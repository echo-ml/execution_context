#pragma once

#include <echo/execution_context/matrix_utility.h>
#include <echo/execution_context/structure.h>
#include <echo/index.h>
#include <mkl.h>
#include <cassert>

namespace echo {
namespace execution_context {
namespace intel_mkl {

using structure::uplo_t;

namespace detail {
namespace blas_executer {

//------------------------------------------------------------------------------
// get_operation
//------------------------------------------------------------------------------
inline CBLAS_TRANSPOSE get_operation(matrix_operation_t operation) {
  switch (operation) {
    case matrix_operation_t::none:
      return CblasNoTrans;
    case matrix_operation_t::transpose:
      return CblasTrans;
    case matrix_operation_t::conjugate_transpose:
      return CblasConjTrans;
  }
}

//------------------------------------------------------------------------------
// get_side
//------------------------------------------------------------------------------
inline CBLAS_SIDE get_side(matrix_side_t side) {
  switch (side) {
    case matrix_side_t::left:
      return CblasLeft;
    case matrix_side_t::right:
      return CblasRight;
  }
}

//------------------------------------------------------------------------------
// get_uplo
//------------------------------------------------------------------------------
inline CBLAS_UPLO get_uplo(uplo_t uplo) {
  switch (uplo) {
    case structure::uplo_t::upper:
      return CblasUpper;
    case structure::uplo_t::lower:
      return CblasLower;
  }
}

//------------------------------------------------------------------------------
// is_unity_fill
//------------------------------------------------------------------------------
inline CBLAS_DIAG is_unity_fill(matrix_diagonal_fill_t diagonal_fill) {
  switch (diagonal_fill) {
    case matrix_diagonal_fill_t::unity:
      return CblasUnit;
    case matrix_diagonal_fill_t::general:
      return CblasNonUnit;
    default:
      assert(0 && "unreachable");
  }
}
}
}

//------------------------------------------------------------------------------
// BlasExecuter
//------------------------------------------------------------------------------
struct BlasExecuter {

//------------------------------------------------------------------------------
// dot
//------------------------------------------------------------------------------
#define MAKE_DOT(SCALAR, PREFIX)                                               \
  auto dot(index_t n, const SCALAR* x, index_t incx, const SCALAR* y,          \
           index_t incy) const {                                               \
    return cblas_##PREFIX##dot(static_cast<int>(n), x, static_cast<int>(incx), \
                               y, static_cast<int>(incy));                     \
  }
  MAKE_DOT(float, s)
  MAKE_DOT(double, d)
#undef MAKE_DOT

//------------------------------------------------------------------------------
// gemv
//------------------------------------------------------------------------------
#define MAKE_GEMV(SCALAR, PREFIX)                                            \
  void gemv(matrix_operation_t operation_a, index_t m, index_t n,            \
            SCALAR alpha, const SCALAR* a, index_t lda, const SCALAR* x,     \
            index_t incx, SCALAR beta, SCALAR* y, index_t incy) const {      \
    using namespace detail::blas_executer;                                   \
    cblas_##PREFIX##gemv(CblasColMajor, get_operation(operation_a),          \
                         static_cast<int>(m), static_cast<int>(n), alpha, a, \
                         static_cast<int>(lda), x, static_cast<int>(incx),   \
                         beta, y, static_cast<int>(incy));                   \
  }
  MAKE_GEMV(float, s)
  MAKE_GEMV(double, d)

#undef MAKE_GEMV


//------------------------------------------------------------------------------
// trsv
//------------------------------------------------------------------------------
#define MAKE_TRSV(SCALAR, PREFIX)                                              \
  void trsv(uplo_t uplo_a, matrix_operation_t operation_a,                     \
            matrix_diagonal_fill_t diagonal_fill_a, index_t n,                 \
            const SCALAR* a, index_t lda, SCALAR* x, index_t incx) const {     \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##trsv(CblasColMajor, get_uplo(uplo_a),                      \
                         get_operation(operation_a),                           \
                         is_unity_fill(diagonal_fill_a), static_cast<int>(n),  \
                         a, static_cast<int>(lda), x, static_cast<int>(incx)); \
  }
  MAKE_TRSV(float, s)
  MAKE_TRSV(double, d)
#undef MAKE_TRSV

//------------------------------------------------------------------------------
// symv
//------------------------------------------------------------------------------
#define MAKE_SYMV(SCALAR, PREFIX)                                              \
  void symv(uplo_t uplo_a, index_t n, SCALAR alpha, const SCALAR* a,           \
            index_t lda, const SCALAR* x, index_t incx, SCALAR beta,           \
            SCALAR* y, index_t incy) const {                                   \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##symv(CblasColMajor, get_uplo(uplo_a), static_cast<int>(n), \
                         alpha, a, static_cast<int>(lda), x,                   \
                         static_cast<int>(incx), beta, y,                      \
                         static_cast<int>(incy));                              \
  }
  MAKE_SYMV(float, s)
  MAKE_SYMV(double, d)
#undef MAKE_SYMV

//------------------------------------------------------------------------------
// syr
//------------------------------------------------------------------------------
#define MAKE_SYR(SCALAR, PREFIX)                                              \
  void syr(uplo_t uplo_a, index_t n, SCALAR alpha, const SCALAR* x,           \
           index_t incx, SCALAR* a, index_t lda) const {                      \
    using namespace detail::blas_executer;                                    \
    cblas_##PREFIX##syr(CblasColMajor, get_uplo(uplo_a), static_cast<int>(n), \
                        alpha, x, static_cast<int>(incx), a,                  \
                        static_cast<int>(lda));                               \
  }
  MAKE_SYR(float, s)
  MAKE_SYR(double, d)
#undef MAKE_SYR

//------------------------------------------------------------------------------
// syr2
//------------------------------------------------------------------------------
#define MAKE_SYR2(SCALAR, PREFIX)                                              \
  void syr2(uplo_t uplo_a, index_t n, SCALAR alpha, const SCALAR* x,           \
            index_t incx, const SCALAR* y, index_t incy, SCALAR* a,            \
            index_t lda) const {                                               \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##syr2(CblasColMajor, get_uplo(uplo_a), static_cast<int>(n), \
                         alpha, x, static_cast<int>(incx), y,                  \
                         static_cast<int>(incy), a, static_cast<int>(lda));    \
  }
  MAKE_SYR2(float, s)
  MAKE_SYR2(double, d)
#undef MAKE_SYR2

//------------------------------------------------------------------------------
// gemm
//------------------------------------------------------------------------------
#define MAKE_GEMM(SCALAR, PREFIX)                                              \
  void gemm(matrix_operation_t operation_a, matrix_operation_t operation_b,    \
            index_t m, index_t n, index_t k, SCALAR alpha, const SCALAR* a,    \
            index_t lda, const SCALAR* b, index_t ldb, SCALAR beta, SCALAR* c, \
            index_t ldc) const {                                               \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##gemm(CblasColMajor, get_operation(operation_a),            \
                         get_operation(operation_b), static_cast<int>(m),      \
                         static_cast<int>(n), static_cast<int>(k), alpha, a,   \
                         static_cast<int>(lda), b, static_cast<int>(ldb),      \
                         beta, c, static_cast<int>(ldc));                      \
  }
  MAKE_GEMM(float, s)
  MAKE_GEMM(double, d)
#undef MAKE_GEMM

//------------------------------------------------------------------------------
// trsm
//------------------------------------------------------------------------------
#define MAKE_TRSM(SCALAR, PREFIX)                                              \
  void trsm(matrix_side_t side, uplo_t uplo_a, matrix_operation_t operation_a, \
            matrix_diagonal_fill_t diagonal_fill_a, index_t m, index_t n,      \
            SCALAR alpha, const SCALAR* a, index_t lda, SCALAR* b,             \
            index_t ldb) const {                                               \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##trsm(CblasColMajor, get_side(side), get_uplo(uplo_a),      \
                         get_operation(operation_a),                           \
                         is_unity_fill(diagonal_fill_a), static_cast<int>(m),  \
                         static_cast<int>(n), alpha, a, static_cast<int>(lda), \
                         b, static_cast<int>(ldb));                            \
  }
  MAKE_TRSM(float, s)
  MAKE_TRSM(double, d)
#undef MAKE_TRSM

//------------------------------------------------------------------------------
// syrk
//------------------------------------------------------------------------------
#define MAKE_SYRK(SCALAR, PREFIX)                                              \
  void syrk(uplo_t uplo_c, matrix_operation_t operation_a, index_t n,          \
            index_t k, SCALAR alpha, const SCALAR* a, index_t lda,             \
            SCALAR beta, SCALAR* c, index_t ldc) const {                       \
    using namespace detail::blas_executer;                                     \
    cblas_##PREFIX##syrk(CblasColMajor, get_uplo(uplo_c),                      \
                         get_operation(operation_a), static_cast<int>(n),      \
                         static_cast<int>(k), alpha, a, static_cast<int>(lda), \
                         beta, c, static_cast<int>(ldc));                      \
  }
  MAKE_SYRK(float, s)
  MAKE_SYRK(double, d)
#undef MAKE_SYRK

//------------------------------------------------------------------------------
// syr2k
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// symm
//------------------------------------------------------------------------------
#define MAKE_SYMM(SCALAR, PREFIX)                                            \
  void symm(matrix_side_t side_a, uplo_t uplo_a, index_t m, index_t n,       \
            SCALAR alpha, const SCALAR* a, index_t lda, const SCALAR* b,     \
            index_t ldb, SCALAR beta, SCALAR* c, index_t ldc) const {        \
    using namespace detail::blas_executer;                                   \
    cblas_##PREFIX##symm(CblasColMajor, get_side(side_a), get_uplo(uplo_a),  \
                         static_cast<int>(m), static_cast<int>(n), alpha, a, \
                         static_cast<int>(lda), b, static_cast<int>(ldb),    \
                         beta, c, static_cast<int>(ldc));                    \
  }
  MAKE_SYMM(float, s)
  MAKE_SYMM(double, d)
#undef MAKE_SYMM
};
}
}
}
