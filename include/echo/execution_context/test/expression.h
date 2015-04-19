#pragma once

#include <echo/k_array.h>
#include <echo/execution_context/concept.h>

namespace echo {
namespace execution_context {
namespace test {

////////////////
// Expression //
////////////////

template <class Shape, class Structure, class Evaluator>
class Expression : Shape {
 public:
  using structure = Structure;
  Expression(const Shape& shape, const Evaluator& evaluator)
      : Shape(shape), _evaluator(evaluator) {}
  const Shape& shape() const { return static_cast<const Shape&>(*this); }
  const Evaluator& evaluator() const { return _evaluator; }

 private:
  Evaluator _evaluator;
};

/////////////////////
// make_expression //
/////////////////////

template <class Structure, class Shape, class Evaluator,
          CONCEPT_REQUIRES(k_array::concept::shape<Shape>() &&
                           execution_context::concept::structure<Structure>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(const Shape& shape, const Evaluator& evaluator) {
  return Expression<Shape, Structure, Evaluator>(shape, evaluator);
}

template <
    class Extent, class Evaluator,
    CONCEPT_REQUIRES(k_array::concept::extent<Extent>() &&
                     execution_context::concept::vector_evaluator<Evaluator>())>
auto make_expression(Extent extent, const Evaluator& evaluator) {
  return make_expression<structure::general>(make_k_shape(extent), evaluator);
}

template <class Extent1, class Extent2, class Evaluator,
          CONCEPT_REQUIRES(k_array::concept::extent<Extent1>() &&
                           k_array::concept::extent<Extent2>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(Extent1 extent1, Extent2 extent2,
                     const Evaluator& evaluator) {
  return make_expression<structure::general>(make_k_shape(extent1, extent2),
                                             evaluator);
}

template <class Structure, class Extent1, class Extent2, class Evaluator,
          CONCEPT_REQUIRES(execution_context::concept::structure<Structure>() &&
                           k_array::concept::extent<Extent1>() &&
                           k_array::concept::extent<Extent2>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(Extent1 extent1, Extent2 extent2,
                     const Evaluator& evaluator) {
  return make_expression<Structure>(make_k_shape(extent1, extent2), evaluator);
}
}
}
}
