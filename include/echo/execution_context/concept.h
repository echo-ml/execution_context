#pragma once

#include <echo/execution_context/structure.h>
#include <echo/execution_context/expression_traits.h>
#include <echo/concept2.h>
#include <echo/k_array.h>
#include <echo/type_traits.h>

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
      -> list<echo::concept::regular<T>(), same<T, decltype(-x)>(),
              same<T, decltype(x + x)>(), same<T, decltype(x - x)>(),
              same<T, decltype(x* x)>(), same<T, decltype(x / x)>()>;
};
}
}

template <class T>
constexpr bool scalar() {
  return models<detail::concept::Scalar, T>();
}

////////////////////
// flat_evaluator //
////////////////////

namespace detail {
namespace concept {
struct FlatEvaluator : Concept {
  template <class T>
  auto require(T&& evaluator)
      -> list<std::is_copy_constructible<T>::value,
              scalar<uncvref_t<std::result_of_t<T(index_t)>>>()>;
};
}
}

template <class T>
constexpr bool flat_evaluator() {
  return models<detail::concept::FlatEvaluator, T>();
}

////////////////////////
// k_shaped_evaluator //
////////////////////////

namespace detail {
namespace concept {
template <std::size_t... Indexes, class Evaluator>
auto evaluator_result(std::index_sequence<Indexes...>,
                      const Evaluator& evaluator)
    -> decltype(evaluator((Indexes, index_t(0))...));

template <int K>
struct KShapedEvaluator : Concept {
  template <class T>
  auto require(T&& evaluator)
      -> list<std::is_copy_constructible<T>::value,
              scalar<uncvref_t<decltype(evaluator_result(
                  std::make_index_sequence<2 * K>(), evaluator))>>()>;
};
}
}

template <int K, class T>
constexpr bool k_shaped_evaluator() {
  return models<detail::concept::KShapedEvaluator<K>, T>();
}

//////////////////////
// matrix_evaluator //
//////////////////////

template <class T>
constexpr bool matrix_evaluator() {
  return k_shaped_evaluator<2, T>();
}

///////////////
// evaluator //
///////////////

template <class T>
constexpr bool evaluator() {
  return matrix_evaluator<T>() || flat_evaluator<T>();
}

/////////////
// reducer //
/////////////

namespace detail {
namespace concept {
struct Reducer : Concept {
  template <class Scalar, class Reducer>
  auto require(Scalar&& x, Reducer&& reducer)
      -> list<std::is_copy_constructible<Reducer>::value, scalar<Scalar>(),
              same<Scalar, decltype(reducer(x, x))>()>;
};
}
}

template <class Scalar, class Reducer>
constexpr bool reducer() {
  return models<detail::concept::Reducer, Scalar, Reducer>();
}

///////////////
// structure //
///////////////

template <class T>
constexpr bool structure() {
  return std::is_convertible<T,
                             echo::execution_context::structure::base>::value;
}

/////////////////////
// flat_expression //
/////////////////////

namespace detail {
namespace concept {
struct FlatExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      flat_evaluator<uncvref_t<decltype(expression.evaluator())>>()>;
};
}
}

template <class T>
constexpr bool flat_expression() {
  return models<detail::concept::FlatExpression, T>();
}

///////////////////////
// shaped_expression //
///////////////////////

namespace detail {
namespace concept {
struct ShapedExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      structure<expression_traits::structure<T>>(),
      k_shaped_evaluator<
          shape_traits::num_dimensions<decltype(expression.shape())>(),
          uncvref_t<decltype(expression.evaluator())>>()>;
};
}
}

template <class T>
constexpr bool shaped_expression() {
  return models<detail::concept::ShapedExpression, T>();
}

//////////////////////////////
// shaped_matrix_expression //
//////////////////////////////

namespace detail {
namespace concept {
struct ShapedMatrixExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      shaped_expression<T>(),
      shape_traits::num_dimensions<decltype(expression.shape())>() == 2>;
};
}
}

template <class T>
constexpr bool shaped_matrix_expression() {
  return models<detail::concept::ShapedMatrixExpression, T>();
}

namespace detail {
namespace concept {

template <structure::uplo_t Uplo, bool Strict>
void match_half_structure(structure::half<Uplo, Strict>);

struct HalfMatrixExpression : Concept {
  template <class T>
  auto require(T&& expression)
      -> list<shaped_matrix_expression<T>(),
              valid<decltype(
                  match_half_structure(expression_traits::structure<T>()))>()>;
};
}
}

template <class T>
constexpr bool half_matrix_expression() {
  return models<detail::concept::HalfMatrixExpression, T>();
}

///////////////////////////////
// flat_reduction_expression //
///////////////////////////////

namespace detail {
namespace concept {
struct FlatReductionExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      flat_evaluator<uncvref_t<decltype(expression.mapper())>>(),
      // broken with intel compiler
      // reducer<uncvref_t<decltype(expression.identity())>,
      //         uncvref_t<decltype(expression.reducer())>>(),
      // same<uncvref_t<decltype(expression.identity())>,
      //      evaluator_traits::value_type<decltype(expression.mapper())>>()
      valid<decltype(expression.identity())>(),
      valid<decltype(expression.reducer())>()
  >;
};
}
}

template <class T>
constexpr bool flat_reduction_expression() {
  return models<detail::concept::FlatReductionExpression, T>();
}

/////////////////////////////////
// shaped_reduction_expression //
/////////////////////////////////

namespace detail {
namespace concept {
struct ShapedReductionExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      k_shaped_evaluator<
          shape_traits::num_dimensions<decltype(expression.shape())>(),
          uncvref_t<decltype(expression.mapper())>>(),
      structure<expression_traits::structure<T>>(),
      // broken with intel compiler
      // reducer<uncvref_t<decltype(expression.identity())>,
      //         uncvref_t<decltype(expression.reducer())>>(),
      // same<uncvref_t<decltype(expression.identity())>,
      //      evaluator_traits::value_type<decltype(expression.mapper())>>()
      valid<decltype(expression.identity())>(),
      valid<decltype(expression.reducer())>()
  >;
};
}
}

template <class T>
constexpr bool shaped_reduction_expression() {
  return models<detail::concept::ShapedReductionExpression, T>();
}

//////////////////////////
// reduction_expression //
//////////////////////////

template <class T>
constexpr bool reduction_expression() {
  return flat_reduction_expression<T>() || shaped_reduction_expression<T>();
}

////////////////
// expression //
////////////////

template <class T>
constexpr bool expression() {
  return flat_expression<T>() || shaped_expression<T>();
}
}
}
}
