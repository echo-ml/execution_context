#pragma once

#include <echo/execution_context/concept.h>
#include <echo/execution_context/option.h>
#include <echo/index.h>

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
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}

// simd
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
#pragma simd
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}

// temporal
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::nosimd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
#pragma vector nontemporal
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}

// simd, nontemporal
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::nonaligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
#pragma simd
#pragma vector nontemporal
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}

// simd, aligned
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::temporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
#pragma simd
#pragma vector aligned
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}

// simd, nontemporal, aligned
template <
    class Options, class Evaluator,
    CONCEPT_REQUIRES(option::concept::option_list<Options>() &&
                     execution_context::concept::vector_evaluator<Evaluator>()),
    CONCEPT_REQUIRES(get_option<execution_mode::simd_t, Options>() ==
                         execution_mode::simd &&
                     get_option<execution_mode::temporal_t, Options>() ==
                         execution_mode::nontemporal &&
                     get_option<execution_mode::aligned_t, Options>() ==
                         execution_mode::aligned)>
void for_(Options, IndexInteger a, IndexInteger b, const Evaluator& evaluator) {
#pragma simd
#pragma vector aligned
#pragma vector nontemporal
  for (IndexInteger i = a; i < b; ++i) evaluator(i);
}
}
}
}
}
