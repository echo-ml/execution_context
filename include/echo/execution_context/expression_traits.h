#pragma once

#include <echo/concept2.h>

namespace echo { namespace expression_traits {

template<class Expression>
using structure = typename uncvref_t<Expression>::structure;

template<class Expression>
constexpr int num_dimensions() {
  using Shape = uncvref_t<decltype(std::declval<Expression>().shape())>;
  return shape_traits::num_dimensions<Shape>();
}

}}
