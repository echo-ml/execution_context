#pragma once

#include <type_traits>

#define DETAIL_NS detail_structure

namespace echo { namespace execution_context {

namespace structure {

struct base {};

enum class uplo_t { upper, lower };

struct general : base {};

struct square {};

template<uplo_t Uplo, bool Strict> struct half : virtual base, virtual square {
  static const uplo_t uplo = Uplo;
  static const bool strict = Strict;
};

using upper_half = half<uplo_t::upper, false>;
using lower_half = half<uplo_t::lower, false>;

using strict_upper_half = half<uplo_t::upper, true>;
using strict_lower_half = half<uplo_t::lower, true>;

namespace concept {

//------------------------------------------------------------------------------
// base
//------------------------------------------------------------------------------
namespace DETAIL_NS {
inline
auto general_impl(structure::general) -> std::true_type;

inline
auto general_impl(...) -> std::false_type;
}

template<class T>
constexpr bool general() {
  using Result = decltype(DETAIL_NS::general_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// half
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template<uplo_t Uplo, bool Strict>
auto half_impl(structure::half<Uplo, Strict>) -> std::true_type;

inline
auto half_impl(...) -> std::false_type;
}

template<class T>
constexpr bool half() {
  using Result = decltype(DETAIL_NS::half_impl(std::declval<T>()));
  return Result::value;
}

}
}
}}

#undef DETAIL_NS
