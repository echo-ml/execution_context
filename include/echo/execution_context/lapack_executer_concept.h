#pragma once

#include <echo/execution_context/matrix_utility.h>
#include <echo/concept.h>

#define DETAIL_NS detail_lapack_executer_concept

namespace echo {
namespace execution_context {
namespace concept {

//------------------------------------------------------------------------------
// lapack_executer
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Scalar>
struct LapackExecuter : Concept {
  template <class T>
  auto require(T&& executer)
      -> list<same<bool, decltype(executer.potrf(structure::uplo_t(), index_t(),
                                                 std::declval<Scalar*>(),
                                                 index_t()))>(),
              same<bool, decltype(executer.potrf(structure::uplo_t(), index_t(),
                                                 std::declval<Scalar*>(),
                                                 index_t()))>()>;
};
}

template <class T>
constexpr bool lapack_executer() {
  return models<DETAIL_NS::LapackExecuter<float>, T>() &&
         models<DETAIL_NS::LapackExecuter<double>, T>();
}
}
}
}
#undef DETAIL_NS
