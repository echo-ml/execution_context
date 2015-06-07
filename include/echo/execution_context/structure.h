#pragma once

namespace echo { namespace execution_context {

namespace structure {

struct base {};

enum class uplo_t { upper, lower };

struct general : base {};

struct square {};

template<uplo_t Uplo, bool Strict> struct half : base, square {
  static const uplo_t uplo = Uplo;
  static const bool strict = Strict;
};

using upper_half = half<uplo_t::upper, false>;
using lower_half = half<uplo_t::lower, false>;

using strict_upper_half = half<uplo_t::upper, true>;
using strict_lower_half = half<uplo_t::lower, true>;

}

}}
