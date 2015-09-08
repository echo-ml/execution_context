#pragma once

#define DETAIL_NS detail_lapack_executer

#include <echo/execution_context/matrix_utility.h>
#include <echo/execution_context/structure.h>
#include <echo/index.h>
#include <mkl.h>

namespace echo {
namespace execution_context {
namespace intel_mkl {

//------------------------------------------------------------------------------
// get_uplo
//------------------------------------------------------------------------------
namespace DETAIL_NS {
inline char get_uplo(structure::uplo_t uplo) {
  switch (uplo) {
    case structure::uplo_t::upper:
      return 'U';
    case structure::uplo_t::lower:
      return 'L';
  }
}
}

//------------------------------------------------------------------------------
// get_operation
//------------------------------------------------------------------------------
namespace DETAIL_NS {
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
// get_result_output_mode
//------------------------------------------------------------------------------
namespace DETAIL_NS {
inline char get_result_output_mode(result_output_mode_t result_output_mode) {
  switch (result_output_mode) {
    case result_output_mode_t::all:
      return 'A';
    case result_output_mode_t::subset:
      return 'S';
    case result_output_mode_t::overwrite:
      return 'O';
    case result_output_mode_t::none:
      return 'N';
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
  int gels(matrix_operation_t operation_a, index_t m, index_t n, index_t nrhs, \
           SCALAR* a, index_t lda, SCALAR* b, index_t ldb) const {             \
    using namespace DETAIL_NS;                                                 \
    auto result = LAPACKE_##SUFFIX##gels(                                      \
        CblasColMajor, get_operation(operation_a), static_cast<int>(m),        \
        static_cast<int>(n), static_cast<int>(nrhs), a, static_cast<int>(lda), \
        b, static_cast<int>(ldb));                                             \
    return static_cast<int>(result);                                           \
  }
  MAKE_GELS(float, s)
  MAKE_GELS(double, d)
#undef MAKE_GELS

//------------------------------------------------------------------------------
// gesvd
//------------------------------------------------------------------------------
#define MAKE_GESVD(SCALAR, SUFFIX)                                     \
  int gesvd(result_output_mode_t u_output_mode,                        \
            result_output_mode_t vt_output_mode, index_t m, index_t n, \
            SCALAR* a, index_t lda, SCALAR* s, SCALAR* u, index_t ldu, \
            SCALAR* vt, index_t ldvt, SCALAR* superb) const {          \
    using namespace DETAIL_NS;                                         \
    auto result = LAPACKE_##SUFFIX##gesvd(                             \
        CblasColMajor, get_result_output_mode(u_output_mode),          \
        get_result_output_mode(vt_output_mode), static_cast<int>(m),   \
        static_cast<int>(n), a, static_cast<int>(lda), s, u,           \
        static_cast<int>(ldu), vt, static_cast<int>(ldvt), superb);    \
    return static_cast<int>(result);                                   \
  }
  MAKE_GESVD(float, s)
  MAKE_GESVD(double, d)
#undef MAKE_GESVD
};
}
}
}

#undef DETAIL_NS
