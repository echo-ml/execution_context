#pragma once

#define DETAIL_NS detail_lapack_executer

#include <echo/execution_context/matrix_utility.h>
#include <echo/execution_context/structure.h>
#include <echo/index.h>
#include <mkl.h>

namespace echo {
namespace execution_context {
namespace intel_mkl {

namespace DETAIL_NS {
//------------------------------------------------------------------------------
// get_uplo
//------------------------------------------------------------------------------
inline char get_uplo(structure::uplo_t uplo) {
  switch (uplo) {
    case structure::uplo_t::upper:
      return 'U';
    case structure::uplo_t::lower:
      return 'L';
  }
}

//------------------------------------------------------------------------------
// get_operation
//------------------------------------------------------------------------------
inline char get_operation(matrix_operation_t operation) {
  switch (operation) {
    case matrix_operation_t::none:
      return 'N';
    case matrix_operation_t::transpose:
      return 'T';
    case matrix_operation_t::conjugate_transpose:
      return 'C';
  }
}
}

//------------------------------------------------------------------------------
// LapackExecuter
//------------------------------------------------------------------------------
struct LapackExecuter {
//------------------------------------------------------------------------------
// potrf
//------------------------------------------------------------------------------
#define MAKE_POTRF(SCALAR, SUFFIX)                                       \
  bool potrf(structure::uplo_t uplo, index_t n, SCALAR* a, index_t lda)  \
      const {                                                            \
    using namespace DETAIL_NS;                                           \
    auto status = LAPACKE_##SUFFIX##potrf(CblasColMajor, get_uplo(uplo), \
                                          static_cast<int>(n), a,        \
                                          static_cast<int>(lda));        \
    return status == 0;                                                  \
  }
  MAKE_POTRF(float, s)
  MAKE_POTRF(double, d)
#undef MAKE_POTRF

//------------------------------------------------------------------------------
// potri
//------------------------------------------------------------------------------
#define MAKE_POTRI(SCALAR, SUFFIX)                                          \
  void potri(structure::uplo_t uplo, index_t n, SCALAR* a, index_t lda)     \
      const {                                                               \
    using namespace DETAIL_NS;                                              \
    LAPACKE_##SUFFIX##potri(CblasColMajor, get_uplo(uplo),                  \
                            static_cast<int>(n), a, static_cast<int>(lda)); \
  }
  MAKE_POTRI(float, s)
  MAKE_POTRI(double, d)
#undef MAKE_POTRI

//------------------------------------------------------------------------------
// gels
//------------------------------------------------------------------------------
#define MAKE_GELS(SCALAR, SUFFIX)                                              \
  lapack_int gels(matrix_operation_t operation_a, index_t m, index_t n,        \
                  index_t nrhs, SCALAR* a, index_t lda, SCALAR* b,             \
                  index_t ldb) const {                                         \
    using namespace DETAIL_NS;                                                 \
    return LAPACKE_##SUFFIX##gels(                                             \
        CblasColMajor, get_operation(operation_a), static_cast<int>(m),        \
        static_cast<int>(n), static_cast<int>(nrhs), a, static_cast<int>(lda), \
        b, static_cast<int>(ldb));                                             \
  }
  MAKE_GELS(float, s)
  MAKE_GELS(double, d)
#undef MAKE_GELS
};
}
}
}

#undef DETAIL_NS
