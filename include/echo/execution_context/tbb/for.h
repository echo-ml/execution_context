#pragma once

#include <echo/execution_context/concept.h>
#include <echo/execution_context/option.h>
#include <echo/index.h>
#include <iostream>

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
#pragma simd
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
#pragma vector nontemporal
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
#pragma simd
#pragma vector nontemporal
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
#pragma simd
#pragma vector aligned
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
#pragma simd
#pragma vector aligned
#pragma vector nontemporal
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
#pragma inline recursive
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
#pragma simd
  for (index_t i = a; i < b; ++i)
#pragma inline recursive
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
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma inline recursive
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
#pragma simd
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma inline recursive
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
#pragma simd
#pragma vector aligned
  for (index_t i = a; i < b; ++i)
#pragma inline recursive
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
#pragma simd
#pragma vector aligned
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma inline recursive
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
#pragma forceinline recursive
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
#pragma simd
  for (index_t i = a; i < b; ++i)
#pragma forceinline recursive
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
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma forceinline recursive
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
#pragma simd
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma forceinline recursive
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
#pragma simd
#pragma vector aligned
  for (index_t i = a; i < b; ++i)
#pragma forceinline recursive
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
#pragma simd
#pragma vector aligned
#pragma vector nontemporal
  for (index_t i = a; i < b; ++i)
#pragma forceinline recursive
    evaluator(i);
}
}
}
}
}
