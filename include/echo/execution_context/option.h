#pragma once

#include <echo/option.h>

namespace echo {
namespace execution_mode {

enum class simd_t { yes, no };
enum class temporal_t { yes, no };
enum class parallel_t { serial, fine, coarse };
enum class aligned_t { yes, no };

enum class inline_t { force, suggest, nosuggest };

constexpr option::Option<simd_t, simd_t::yes> simd{};
constexpr option::Option<simd_t, simd_t::no> no_simd{};

constexpr option::Option<temporal_t, temporal_t::yes> temporal{};
constexpr option::Option<temporal_t, temporal_t::no> nontemporal{};

constexpr option::Option<parallel_t, parallel_t::serial> serial{};
constexpr option::Option<parallel_t, parallel_t::fine> parallel{};
constexpr option::Option<parallel_t, parallel_t::fine> parallel_fine{};
constexpr option::Option<parallel_t, parallel_t::coarse> parallel_coarse{};

constexpr option::Option<aligned_t, aligned_t::yes> aligned{};
constexpr option::Option<aligned_t, aligned_t::no> nonaligned{};

constexpr option::Option<inline_t, inline_t::force> force_inline{};
constexpr option::Option<inline_t, inline_t::suggest> suggest_inline{};
constexpr option::Option<inline_t, inline_t::nosuggest> nosuggest_inline{};

}
}
