#pragma once

#include <echo/concept.h>
#include <echo/type_traits.h>
#include <echo/k_array.h>

namespace echo {
namespace evaluator_traits {
//------------------------------------------------------------------------------
// value_type
//------------------------------------------------------------------------------
template <class Evaluator>
using value_type = uncvref_t<type_traits::function_return_type<decltype(
    &uncvref_t<Evaluator>::operator())>>;
}

namespace expression_traits {

//------------------------------------------------------------------------------
// evaluator_type
//------------------------------------------------------------------------------
template <class Expression>
using evaluator_type =
    uncvref_t<decltype(std::declval<Expression>().evaluator())>;

//------------------------------------------------------------------------------
// value_type
//------------------------------------------------------------------------------
template <class Expression>
using value_type = evaluator_traits::value_type<decltype(
    std::declval<Expression>().evaluator())>;

//------------------------------------------------------------------------------
// structure
//------------------------------------------------------------------------------
template <class Expression>
using structure = typename uncvref_t<Expression>::structure;

//------------------------------------------------------------------------------
// dimensionality_type
//------------------------------------------------------------------------------
template <class Expression>
using dimensionality_type =
    uncvref_t<decltype(std::declval<Expression>().dimensionality())>;

//------------------------------------------------------------------------------
// num_dimensions
//------------------------------------------------------------------------------

// sfinae friendly version doesn't work on intel compiler
// template <class Expression>
// constexpr auto num_dimensions()
//     -> decltype(shape_traits::num_dimensions<shape_type<Expression>>()) {
//   return shape_traits::num_dimensions<shape_type<Expression>>();
// }
// template <class Expression>
// constexpr int num_dimensions() {
//   return shape_traits::num_dimensions<shape_type<Expression>>();
// }

template<class Expression>
constexpr int num_dimensions() {
  return dimensionality_traits::num_dimensions<
      dimensionality_type<Expression>>();
}

}

namespace reduction_expression_traits {
//------------------------------------------------------------------------------
// value_type
//------------------------------------------------------------------------------
template <class Expression>
using value_type =
    evaluator_traits::value_type<decltype(std::declval<Expression>().mapper())>;

//------------------------------------------------------------------------------
// structure
//------------------------------------------------------------------------------
template <class Expression>
using structure = typename uncvref_t<Expression>::structure;

//------------------------------------------------------------------------------
// dimensionality_type
//------------------------------------------------------------------------------
using expression_traits::dimensionality_type;

//------------------------------------------------------------------------------
// num_dimensions
//------------------------------------------------------------------------------
using expression_traits::num_dimensions;

}
}
