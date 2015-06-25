#pragma once

#include <echo/execution_context/concept.h>
#include <echo/execution_context/option.h>
#include <echo/index.h>
#include <iostream>

#ifdef __INTEL_COMPILER
#define ECHO_PRAGMA_NONTEMPORAL _Pragma("vector nontemporal")
#define ECHO_PRAGMA_SIMD _Pragma("simd")
#define ECHO_PRAGMA_ALIGNED _Pragma("vector aligned")
#define ECHO_PRAGMA_SUGGEST_INLINE _Pragma("inline recursive")
#define ECHO_PRAGMA_FORCE_INLINE _Pragma("forceinline recursive")
#else
#define ECHO_PRAGMA_NONTEMPORAL 
#define ECHO_PRAGMA_SIMD _Pragma("omp simd")
#define ECHO_PRAGMA_ALIGNED
#define ECHO_PRAGMA_SUGGEST_INLINE
#define ECHO_PRAGMA_FORCE_INLINE
#endif

namespace echo {
namespace execution_context {
namespace intel_tbb {

//////////
// for_ //
//////////

namespace detail {

// none
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// simd
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// temporal
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// simd, nontemporal
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// simd, aligned
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// simd, nontemporal, aligned
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::nosuggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i) evaluator(i);
}

// suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// simd, suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// nontemporal, suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// simd, nontemporal, suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// simd, aligned, suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// simd, nontemporal, aligned, suggest_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::suggest_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_SUGGEST_INLINE
    evaluator(i);
}

// force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}

// simd, force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}

// temporal, force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}

// simd, nontemporal, force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}

// simd, aligned, force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}

// simd, nontemporal, aligned, force_inline
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::flat_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned &&
                     get_option<execution_mode::inline_t, Options>() ==
                         execution_mode::force_inline)>
void for_(Options, index_t a, index_t b, const Evaluator& evaluator) {
ECHO_PRAGMA_SIMD
ECHO_PRAGMA_ALIGNED
ECHO_PRAGMA_NONTEMPORAL
  for (index_t i = a; i < b; ++i)
ECHO_PRAGMA_FORCE_INLINE
    evaluator(i);
}
}
}
}
}

#undef ECHO_PRAGMA_NONTEMPORAL
#undef ECHO_PRAGMA_SIMD
#undef ECHO_PRAGMA_ALIGNED
#undef ECHO_PRAGMA_SUGGEST_INLINE
#undef ECHO_PRAGMA_FORCE_INLINE
