#pragma once

#include <echo/k_array.h>
#include <echo/execution_context/concept.h>
#include <echo/type_traits.h>
#include <echo/htl.h>

namespace echo {
namespace execution_context {

//------------------------------------------------------------------------------
// Expression
//------------------------------------------------------------------------------
template <class Dimensionality, class Structure, class Evaluator>
class Expression : htl::Pack<Dimensionality> {
 public:
  using structure = Structure;
  Expression(const Dimensionality& dimensionality, const Evaluator& evaluator)
      : htl::Pack<Dimensionality>(dimensionality), _evaluator(evaluator) {}
  const Dimensionality& dimensionality() const { return htl::unpack(*this); }
  const Evaluator& evaluator() const { return _evaluator; }

 private:
  Evaluator _evaluator;
};

//------------------------------------------------------------------------------
// make_expression
//------------------------------------------------------------------------------
template <class Structure, class Dimensionality, class Evaluator,
          CONCEPT_REQUIRES(k_array::concept::dimensionality<Dimensionality>() &&
                           execution_context::concept::structure<Structure>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(const Dimensionality& dimensionality,
                     const Evaluator& evaluator) {
  return Expression<Dimensionality, Structure, Evaluator>(dimensionality,
                                                          evaluator);
}

template <
    class Extent, class Evaluator,
    CONCEPT_REQUIRES(k_array::concept::extent<Extent>() &&
                     execution_context::concept::flat_evaluator<Evaluator>())>
auto make_expression(Extent extent, const Evaluator& evaluator) {
  return make_expression<structure::general>(make_dimensionality(extent),
                                             evaluator);
}

template <class Extent1, class Extent2, class Evaluator,
          CONCEPT_REQUIRES(k_array::concept::extent<Extent1>() &&
                           k_array::concept::extent<Extent2>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(Extent1 extent1, Extent2 extent2,
                     const Evaluator& evaluator) {
  return make_expression<structure::general>(
      make_dimensionality(extent1, extent2), evaluator);
}

template <class Structure, class Extent1, class Extent2, class Evaluator,
          CONCEPT_REQUIRES(execution_context::concept::structure<Structure>() &&
                           k_array::concept::extent<Extent1>() &&
                           k_array::concept::extent<Extent2>() &&
                           execution_context::concept::evaluator<Evaluator>())>
auto make_expression(Extent1 extent1, Extent2 extent2,
                     const Evaluator& evaluator) {
  return make_expression<Structure>(make_dimensionality(extent1, extent2),
                                    evaluator);
}

//------------------------------------------------------------------------------
// ReductionExpression
//------------------------------------------------------------------------------
template <class Dimensionality, class Structure, class Mapper, class Reducer>
class ReductionExpression : Dimensionality {
  using Scalar =
      type_traits::function_return_type<decltype(&Mapper::operator())>;

 public:
  using structure = Structure;
  ReductionExpression(const Dimensionality& dimensionality,
                      const Mapper& mapper, const Reducer& reducer,
                      Scalar identity)
      : Dimensionality(dimensionality),
        _mapper(mapper),
        _reducer(reducer),
        _identity(identity) {}
  const Dimensionality& dimensionality() const {
    return static_cast<const Dimensionality&>(*this);
  }
  const Mapper& mapper() const { return _mapper; }
  const Reducer& reducer() const { return _reducer; }
  Scalar identity() const { return _identity; }

 private:
  Mapper _mapper;
  Reducer _reducer;
  Scalar _identity;
};

//------------------------------------------------------------------------------
// make_reduction_expression
//------------------------------------------------------------------------------
template <
    class Dimensionality, class Mapper, class Reducer,
    CONCEPT_REQUIRES(k_array::concept::dimensionality<Dimensionality>()),
    CONCEPT_REQUIRES(
        concept::flat_evaluator<Mapper>() ||
        concept::k_evaluator<
            dimensionality_traits::num_dimensions<Dimensionality>(), Mapper>()),
    CONCEPT_REQUIRES(concept::reduction_expression<ReductionExpression<
        Dimensionality, structure::general, Mapper, Reducer>>())>
auto make_reduction_expression(
    const Dimensionality& dimensionality, const Mapper& mapper,
    const Reducer& reducer,
    type_traits::function_return_type<decltype(&Mapper::operator())> identity) {
  return ReductionExpression<Dimensionality, structure::general, Mapper,
                             Reducer>(dimensionality, mapper, reducer,
                                      identity);
}

template <class Extent, class Mapper, class Reducer,
          CONCEPT_REQUIRES(k_array::concept::extent<Extent>() &&
                           concept::flat_evaluator<Mapper>()),
          CONCEPT_REQUIRES(concept::flat_evaluator_reduction_expression<
              ReductionExpression<Dimensionality<k_array::extent_type<Extent>>,
                                  structure::general, Mapper, Reducer>>())>
auto make_reduction_expression(Extent extent, const Mapper& mapper,
                               const Reducer& reducer,
                               std::result_of_t<Mapper(index_t)> identity) {
  return make_reduction_expression(make_dimensionality(extent), mapper, reducer,
                                   identity);
}
}
}
