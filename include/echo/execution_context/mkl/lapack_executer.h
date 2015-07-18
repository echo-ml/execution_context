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
};
}
}
}

#undef DETAIL_NS
