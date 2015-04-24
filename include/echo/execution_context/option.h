#pragma once

#include <echo/utility.h>

namespace echo {
namespace execution_mode {

enum class simd_t { yes, no };
enum class temporal_t { yes, no };
enum class parallel_t { serial, fine, coarse };
enum class aligned_t { yes, no };

constexpr option::Option<simd_t, simd_t::yes> simd{};
constexpr option::Option<simd_t, simd_t::no> nosimd{};

constexpr option::Option<temporal_t, temporal_t::yes> temporal{};
constexpr option::Option<temporal_t, temporal_t::no> nontemporal{};

constexpr option::Option<parallel_t, parallel_t::serial> serial{};
constexpr option::Option<parallel_t, parallel_t::fine> parallel{};
constexpr option::Option<parallel_t, parallel_t::fine> parallel_fine{};
constexpr option::Option<parallel_t, parallel_t::coarse> parallel_coarse{};

constexpr option::Option<aligned_t, aligned_t::yes> aligned{};
constexpr option::Option<aligned_t, aligned_t::no> nonaligned{};

}
}
