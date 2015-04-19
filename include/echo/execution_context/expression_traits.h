#pragma once

#include <echo/concept2.h>

namespace echo { namespace expression_traits {

template<class Expression>
using structure = typename uncvref_t<Expression>::structure;

}}
