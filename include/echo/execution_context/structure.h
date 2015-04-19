#pragma once

namespace echo { namespace execution_context {

namespace structure {

struct base {};

enum class uplo_t { Upper, Lower };

struct general : base {};

template<uplo_t Uplo, bool Strict> struct half : base {
  static const uplo_t uplo = Uplo;
  static const bool strict = Strict;
};

using upper_half = half<uplo_t::Upper, false>;
using lower_half = half<uplo_t::Lower, false>;

using strict_upper_half = half<uplo_t::Upper, true>;
using strict_lower_half = half<uplo_t::Lower, true>;

}

}}
