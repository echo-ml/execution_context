#pragma once

#include <echo/execution_context/structure.h>
#include <echo/execution_context/allocation_backend.h>
#include <echo/execution_context/expression_traits.h>
#include <echo/concept2.h>
#include <echo/k_array.h>
#include <echo/type_traits.h>
#include <complex>

namespace echo {
namespace execution_context {
namespace concept {

////////////
// scalar //
////////////

template <class T>
constexpr bool scalar() {
  return std::is_integral<T>::value || std::is_floating_point<T>::value ||
         std::is_same<T, std::complex<float>>::value ||
         std::is_same<T, std::complex<double>>::value;
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

// hack to work around bug with intel c++ compiler
auto evaluator_result(...) -> std::nullptr_t;

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

// rewrite concept in this weird way to avoid a bug with intel c++ compiler
template <int K, class T>
constexpr bool k_shaped_evaluator() {
  using EvaluatorResult = decltype(detail::concept::evaluator_result(
      std::make_index_sequence<2 * K>(), std::declval<const T>()));
  return std::is_copy_constructible<T>::value &&
         scalar<uncvref_t<EvaluatorResult>>();
}

//////////////////////////
// compatible_evaluator //
//////////////////////////

template <int K, class T>
constexpr bool k_compatible_evaluator() {
  return flat_evaluator<T>() || k_shaped_evaluator<K, T>();
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

///////////////////////////////
// flat_evaluator_expression //
///////////////////////////////

namespace detail {
namespace concept {
struct FlatEvaluatorExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      std::is_copy_constructible<T>::value,
      k_array::concept::shape<uncvref_t<decltype(expression.shape())>>(),
      flat_evaluator<uncvref_t<decltype(expression.evaluator())>>()>;
};
}
}

template <class T>
constexpr bool flat_evaluator_expression() {
  return models<detail::concept::FlatEvaluatorExpression, T>();
}

/////////////////////////////////
// shaped_evaluator_expression //
/////////////////////////////////

namespace detail {
namespace concept {
struct ShapedEvaluatorExpression : Concept {
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
constexpr bool shaped_evaluator_expression() {
  return models<detail::concept::ShapedEvaluatorExpression, T>();
}

//////////////////////////////
// shaped_matrix_expression //
//////////////////////////////

namespace detail {
namespace concept {
struct ShapedMatrixExpression : Concept {
  template <class T>
  auto require(T&& expression) -> list<
      shaped_evaluator_expression<T>(),
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

/////////////////////////////////////////
// flat_evaluator_reduction_expression //
/////////////////////////////////////////

namespace detail {
namespace concept {
struct FlatEvaluatorReductionExpression : Concept {
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
      valid<decltype(expression.reducer())>()>;
};
}
}

template <class T>
constexpr bool flat_evaluator_reduction_expression() {
  return models<detail::concept::FlatEvaluatorReductionExpression, T>();
}

///////////////////////////////////////////
// shaped_evaluator_reduction_expression //
///////////////////////////////////////////

namespace detail {
namespace concept {
struct ShapedEvaluatorReductionExpression : Concept {
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
      valid<decltype(expression.reducer())>()>;
};
}
}

template <class T>
constexpr bool shaped_evaluator_reduction_expression() {
  return models<detail::concept::ShapedEvaluatorReductionExpression, T>();
}

//////////////////////////
// reduction_expression //
//////////////////////////

template <class T>
constexpr bool reduction_expression() {
  return flat_evaluator_reduction_expression<T>() ||
         shaped_evaluator_reduction_expression<T>();
}

////////////////
// expression //
////////////////

template <class T>
constexpr bool expression() {
  return flat_evaluator_expression<T>() || shaped_evaluator_expression<T>();
}

/////////////////////////
// expression_executer //
/////////////////////////

namespace detail {
namespace concept {

struct TestEvaluator1 {
  double operator()(index_t) const;
};

struct TestEvaluator2 {
  double operator()(index_t, index_t, index_t, index_t) const;
};

struct TestExpression1 {
  using structure = structure::general;
  KShape<2, 2> shape() const;
  TestEvaluator1 evaluator() const;
};

struct TestExpression2 {
  using structure = structure::general;
  KShape<2, 2> shape() const;
  TestEvaluator2 evaluator() const;
};

struct ExpressionExecuter : Concept {
  template <class T>
  auto require(T&& executer)
      -> list<valid<decltype(executer(TestExpression1()))>(),
              valid<decltype(executer(TestExpression2()))>()>;
};
}
}

template <class T>
constexpr bool expression_executer() {
  return models<detail::concept::ExpressionExecuter, T>();
}

////////////////////////
// allocation_backend //
////////////////////////

namespace detail {
namespace concept {
struct AllocationBackend : Concept {
  template <class T>
  auto require(T&& allocation_backend)
      -> list<echo::concept::allocator<
                  decltype(make_allocator<double>(allocation_backend))>(),
              echo::concept::allocator<decltype(
                  make_aligned_allocator<double>(allocation_backend))>()>;
};
}
}

template <class T>
constexpr bool allocation_backend() {
  return models<detail::concept::AllocationBackend, T>();
}
}
}
}
