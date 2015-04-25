#pragma once

#include <echo/concept2.h>

namespace echo { namespace expression_traits {

template<class Expression>
using value_type = uncvref_t<decltype(std::declval<Expression>().mapper()(0))>;

template<class Expression>
using structure = typename uncvref_t<Expression>::structure;

}}
