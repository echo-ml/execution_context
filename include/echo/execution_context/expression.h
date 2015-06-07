#pragma once

#include <echo/k_array.h>
#include <echo/execution_context/concept.h>
#include <echo/type_traits.h>

namespace echo {
namespace execution_context {

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
                     execution_context::concept::flat_evaluator<Evaluator>())>
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

/////////////////////////
// ReductionExpression //
/////////////////////////

template <class Shape, class Structure, class Mapper, class Reducer>
class ReductionExpression : Shape {
  using Scalar =
      type_traits::function_return_type<decltype(&Mapper::operator())>;

 public:
  using structure = Structure;
  ReductionExpression(const Shape& shape, const Mapper& mapper,
                      const Reducer& reducer, Scalar identity)
      : Shape(shape), _mapper(mapper), _reducer(reducer), _identity(identity) {}
  const Shape& shape() const { return static_cast<const Shape&>(*this); }
  const Mapper& mapper() const { return _mapper; }
  const Reducer& reducer() const { return _reducer; }
  Scalar identity() const { return _identity; }

 private:
  Mapper _mapper;
  Reducer _reducer;
  Scalar _identity;
};

///////////////////////////////
// make_reduction_expression //
///////////////////////////////

template <
    class Shape, class Mapper, class Reducer,
    CONCEPT_REQUIRES(k_array::concept::shape<Shape>()),
    CONCEPT_REQUIRES(concept::flat_evaluator<Mapper>() ||
                     concept::k_shaped_evaluator<
                         shape_traits::num_dimensions<Shape>(), Mapper>()),
    CONCEPT_REQUIRES(concept::reduction_expression<
        ReductionExpression<Shape, structure::general, Mapper, Reducer>>())>
auto make_reduction_expression(
    const Shape& shape, const Mapper& mapper, const Reducer& reducer,
    type_traits::function_return_type<decltype(&Mapper::operator())> identity) {
  return ReductionExpression<Shape, structure::general, Mapper, Reducer>(
      shape, mapper, reducer, identity);
}

template <class Extent, class Mapper, class Reducer,
          CONCEPT_REQUIRES(k_array::concept::extent<Extent>() &&
                           concept::flat_evaluator<Mapper>()),
          CONCEPT_REQUIRES(concept::flat_evaluator_reduction_expression<
              ReductionExpression<KShapeFromExtents<Extent>, structure::general,
                                  Mapper, Reducer>>())>
auto make_reduction_expression(Extent extent, const Mapper& mapper,
                               const Reducer& reducer,
                               std::result_of_t<Mapper(index_t)> identity) {
  return make_reduction_expression(make_k_shape(extent), mapper, reducer,
                                   identity);
}
}
}