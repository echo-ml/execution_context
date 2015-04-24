#pragma once

#include <echo/execution_context/structure.h>
#include <echo/execution_context/expression_traits.h>
#include <echo/concept2.h>
#include <echo/k_array.h>

namespace echo {
namespace execution_context {
namespace concept {

////////////
// scalar //
////////////

namespace detail {
namespace concept {
struct Scalar : Concept {
  template <class T>
  auto require(T&& x)
      -> list<std::is_pod<T>::value,
              // std::is_trivially_echo::concept::copyable<T>::value,
              same<T, decltype(-x)>(), same<T, decltype(x + x)>(),
              same<T, decltype(x - x)>(), same<T, decltype(x* x)>(),
              same<T, decltype(x / x)>()>;
};
}
}

template <class T>
constexpr bool scalar() {
  return models<detail::concept::Scalar, T>();
}

//////////////////////
// matrix_evaluator //
//////////////////////

namespace detail {
namespace concept {
struct MatrixEvaluator : Concept {
  template <class T>
  auto require(T&& evaluator)
      -> list<std::is_copy_constructible<T>::value,
              scalar<uncvref_t<
                  std::result_of_t<T(index_t, index_t, index_t, index_t)>>>()>;
};
}
}

template <class T>
constexpr bool matrix_evaluator() {
  return models<detail::concept::MatrixEvaluator, T>();
}

//////////////////////
// vector_evaluator //
//////////////////////

namespace detail {
namespace concept {
struct VectorEvaluator : Concept {
  template <class T>
  auto require(T&& evaluator)
      -> list<std::is_copy_constructible<T>::value,
              scalar<uncvref_t<std::result_of_t<T(index_t)>>>()>;
};
}
}

template <class T>
constexpr bool vector_evaluator() {
  return models<detail::concept::VectorEvaluator, T>();
}

///////////////
// evaluator //
///////////////

template <class T>
constexpr bool evaluator() {
  return matrix_evaluator<T>() || vector_evaluator<T>();
}

///////////////
// structure //
///////////////

template <class T>
constexpr bool structure() {
  return std::is_convertible<T,
                             echo::execution_context::structure::base>::value;
}

///////////////////////
// matrix_expression //
///////////////////////

namespace detail {
namespace concept {
struct MatrixExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      structure<expression_traits::structure<T>>(),
      matrix_evaluator<uncvref_t<decltype(expression.evaluator())>>()>;
};
}
}

template <class T>
constexpr bool matrix_expression() {
  return models<detail::concept::MatrixExpression, T>();
}

namespace detail {
namespace concept {

template <structure::uplo_t Uplo, bool Strict>
void match_half_structure(structure::half<Uplo, Strict>);

struct HalfMatrixExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      matrix_expression<T>(), valid<decltype(match_half_structure(
                                  expression_traits::structure<T>()))>()>;
};
}
}

template <class T>
constexpr bool half_matrix_expression() {
  return models<detail::concept::HalfMatrixExpression, T>();
}

///////////////////////
// vector_expression //
///////////////////////

namespace detail {
namespace concept {
struct VectorExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      vector_evaluator<uncvref_t<decltype(expression.evaluator())>>()>;
};
}
}

template <class T>
constexpr bool vector_expression() {
  return models<detail::concept::VectorExpression, T>();
}

////////////////
// expression //
////////////////

template <class T>
constexpr bool expression() {
  return matrix_expression<T>() || vector_expression<T>();
}
}
}
}
