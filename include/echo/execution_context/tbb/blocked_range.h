#pragma once

#include <echo/concept2.h>
#include <tbb/tbb.h>
#include <cassert>
#include <utility>

namespace echo {
namespace execution_context {
namespace intel_tbb {

namespace concept {

//////////////////
// split_factor //
//////////////////

template <class T>
constexpr bool split_factor() {
  return std::is_same<T, tbb::split>::value ||
         std::is_same<T, tbb::proportional_split>::value;
}

////////////
// extent //
////////////

template <class T>
constexpr bool extent() {
  return std::is_integral<T>::value;
}

///////////////////
// blocked_range //
///////////////////

namespace detail {
namespace blocked_range {
struct BlockedRange : Concept {
  template <class T>
  auto require(T&& r) -> list<
      std::is_copy_constructible<T>::value, std::is_destructible<T>::value,
      same<decltype(std::declval<const T&>().is_divisible()), bool>(),
      same<decltype(std::declval<const T&>().empty()), bool>(),
      std::is_constructible<T, T&, tbb::split>::value>;
};
}
}

template <class T>
constexpr bool blocked_range() {
  return models<detail::blocked_range::BlockedRange, T>();
}
}

//////////////////
// BlockedRange //
//////////////////

template <class Extent>
class BlockedRange {
  static_assert(concept::extent<Extent>(), "invalid extent type");

 public:
  using const_iterator = Extent;
  using size_type = std::size_t;
  static const bool is_splittable_in_proportion = true;

  BlockedRange() = default;
  BlockedRange(Extent first, Extent last, size_type grainsize = 1)
      : _first(first), _last(last), _grainsize(grainsize) {}
  template <class SplitFactor,
            CONCEPT_REQUIRES(concept::split_factor<SplitFactor>())>
  BlockedRange(BlockedRange& other, const SplitFactor& split_factor)
      : _last(other._last),
        _first(left_split(split_factor, other)),
        _grainsize(other._grainsize) {}

  size_type size() const {
    assert(_first < _last);
    return _last - _first;
  }
  size_type grainsize() const { return _grainsize; }
  bool empty() const { return !(_first < _last); }
  bool is_divisible() const { return _grainsize < size(); }
  const_iterator begin() const { return _first; }
  const_iterator end() const { return _last; }
  void set_first(Extent first) { _first = first; }
  void set_last(Extent last) { return _last = last; }

  friend Extent left_split(tbb::split, BlockedRange& blocked_range) {
    assert(blocked_range.is_divisible());
    Extent split_point = blocked_range._first +
                         (blocked_range._last - blocked_range._first) / 2u;
    blocked_range._last = split_point;
    return split_point;
  }
  friend Extent left_split(const tbb::proportional_split& proportional_split,
                           BlockedRange& blocked_range) {
    assert(blocked_range.is_divisible());
    size_type right_split_size = size_type(
        float(blocked_range.size()) * float(proportional_split.right()) /
            float(proportional_split.left() + proportional_split.right()) +
        0.5f);
    blocked_range._last = Extent(blocked_range._last - right_split_size);
    return blocked_range._last;
  }

 private:
  Extent _last, _first;
  size_type _grainsize;
};

CONCEPT_ASSERT(concept::blocked_range<BlockedRange<std::size_t>>());

////////////////
// ExtentType //
////////////////

namespace detail {
namespace blocked_range {

template <int I, class... Extents>
struct ExtentTypeImpl {};

template <int I, class Extent>
struct ExtentTypeImpl<I, Extent> {
  using type = Extent;
};

template <int I, class ExtentFirst, class... ExtentsRest>
struct ExtentTypeImpl<I, ExtentFirst, ExtentsRest...> {
  using type =
      std::conditional_t<I == 0, ExtentFirst,
                         typename ExtentTypeImpl<I - 1, ExtentsRest...>::type>;
};

template <int I, class... Extents>
using ExtentType = typename ExtentTypeImpl<I, Extents...>::type;
}
}

///////////////////////
// BlockedRangeTuple //
///////////////////////

namespace detail {
namespace blocked_range {

template <class, class...>
struct BlockedRangeTupleImpl {};

template <std::size_t... Indexes, class... Extents>
struct BlockedRangeTupleImpl<std::index_sequence<Indexes...>, Extents...> {
  using type = std::tuple<BlockedRange<ExtentType<Indexes, Extents...>>...>;
};

template <std::size_t N, class... Extents>
using BlockedRangeTuple =
    typename BlockedRangeTupleImpl<std::make_index_sequence<N>,
                                   Extents...>::type;
}
}

///////////
// split //
///////////

namespace detail {
namespace blocked_range {

template <class SplitFactor, std::size_t I, class... Extents>
void split_impl(const SplitFactor& split_factor, std::index_sequence<I>,
                std::tuple<BlockedRange<Extents>...>& left_blocked_ranges,
                std::tuple<BlockedRange<Extents>...>& right_blocked_ranges) {
  auto& left_blocked_range = std::get<I>(left_blocked_ranges);
  auto& right_blocked_range = std::get<I>(right_blocked_ranges);
  auto split_point = left_split(split_factor, left_blocked_range);
  right_blocked_range.set_first(split_point);
}

template <class SplitFactor, std::size_t I1, std::size_t I2,
          std::size_t... IRest, class... Extents>
void split_impl(const SplitFactor& split_factor,
                std::index_sequence<I1, I2, IRest...>,
                std::tuple<BlockedRange<Extents>...>& left_blocked_ranges,
                std::tuple<BlockedRange<Extents>...>& right_blocked_ranges) {
  auto& blocked_range1 = std::get<I1>(left_blocked_ranges);
  auto& blocked_range2 = std::get<I2>(left_blocked_ranges);
  if (blocked_range1.size() * blocked_range2.grainsize() <
      blocked_range2.size() * blocked_range1.grainsize())
    split_impl(split_factor, std::index_sequence<I2, IRest...>(),
               left_blocked_ranges, right_blocked_ranges);
  else
    split_impl(split_factor, std::index_sequence<I1, IRest...>(),
               left_blocked_ranges, right_blocked_ranges);
}

template <
    class SplitFactor, class... Extents,
    CONCEPT_REQUIRES(concept::split_factor<SplitFactor>() &&
                     const_algorithm::and_c<concept::extent<Extents>()...>())>
void split(const SplitFactor& split_factor,
           std::tuple<BlockedRange<Extents>...>& left_blocked_ranges,
           std::tuple<BlockedRange<Extents>...>& right_blocked_ranges) {
  split_impl(split_factor, std::index_sequence_for<Extents...>(),
             left_blocked_ranges, right_blocked_ranges);
}
}
}

///////////////
// get_first //
///////////////

namespace detail {
namespace blocked_range {

template <int I, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(I == 0)>
auto get_extent_from_index(ExtentFirst extent_first, ExtentsRest...) {
  return extent_first;
}

template <int I, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(I > 0)>
auto get_extent_from_index(ExtentFirst, ExtentsRest... extents_rest) {
  return get_extent_from_index<I - 1>(extents_rest...);
}

template <int I, class... Extents,
          CONCEPT_REQUIRES(
              sizeof...(Extents) % 2 == 0 && I < sizeof...(Extents) / 2 &&
              const_algorithm::and_c<concept::extent<Extents>()...>())>
auto get_first(Extents... extents) {
  return get_extent_from_index<2 * I>(extents...);
}

template <int I, class... Extents,
          CONCEPT_REQUIRES(
              sizeof...(Extents) % 2 == 0 && I < sizeof...(Extents) / 2 &&
              const_algorithm::and_c<concept::extent<Extents>()...>())>
auto get_last(Extents... extents) {
  return get_extent_from_index<2 * I + 1>(extents...);
}
}
}

/////////
// or_ //
/////////

namespace detail {
namespace blocked_range {

bool or_impl() { return false; }

template <class... BoolsRest>
bool or_impl(bool bool_first, BoolsRest... bools_rest) {
  return bool_first || or_impl(bools_rest...);
}

template <class... Bools, CONCEPT_REQUIRES(const_algorithm::and_c<
                              std::is_convertible<Bools, bool>::value...>())>
bool or_(Bools... bools) {
  return or_impl(bools...);
}
}
}

//////////////
// is_empty //
//////////////

namespace detail {
namespace blocked_range {
template <std::size_t... Indexes, class... Extents>
bool is_empty_impl(std::index_sequence<Indexes...>,
                   const std::tuple<BlockedRange<Extents>...>& blocked_ranges) {
  return or_(std::get<Indexes>(blocked_ranges).empty()...);
}
template <class... Extents, CONCEPT_REQUIRES(const_algorithm::and_c<
                                concept::extent<Extents>()...>())>
bool is_empty(const std::tuple<BlockedRange<Extents>...>& blocked_ranges) {
  return is_empty_impl(std::index_sequence_for<Extents...>(), blocked_ranges);
}
}
}

//////////////////
// is_divisible //
//////////////////

namespace detail {
namespace blocked_range {
template <std::size_t... Indexes, class... Extents>
bool is_divisible_impl(
    std::index_sequence<Indexes...>,
    const std::tuple<BlockedRange<Extents>...>& blocked_ranges) {
  return or_(std::get<Indexes>(blocked_ranges).is_divisible()...);
}
template <class... Extents, CONCEPT_REQUIRES(const_algorithm::and_c<
                                concept::extent<Extents>()...>())>
bool is_divisible(const std::tuple<BlockedRange<Extents>...>& blocked_ranges) {
  return is_divisible_impl(std::index_sequence_for<Extents...>(),
                           blocked_ranges);
}
}
}

///////////////////
// KBlockedRange //
///////////////////

template <int N, class... Extents>
class KBlockedRange {
  static_assert(sizeof...(Extents) > 0 && sizeof...(Extents) <= N, "");
  static_assert(const_algorithm::and_c<concept::extent<Extents>()...>(),
                "invalid extent type");

 public:
  static const bool is_splittable_in_proportion = true;

  KBlockedRange() = default;
  template <class... ExtentsInit,
            CONCEPT_REQUIRES(
                sizeof...(ExtentsInit) == 2 * N &&
                const_algorithm::and_c<concept::extent<ExtentsInit>()...>())>
  KBlockedRange(ExtentsInit... extents)
      : KBlockedRange(std::make_index_sequence<N>(), extents...) {}
  template <class SplitFactor,
            CONCEPT_REQUIRES(concept::split_factor<SplitFactor>())>
  KBlockedRange(KBlockedRange& other, const SplitFactor& split_factor)
      : _blocked_ranges(other._blocked_ranges) {
    detail::blocked_range::split(split_factor, other._blocked_ranges,
                                 _blocked_ranges);
  }
  template <class... BlockedRanges,
            CONCEPT_REQUIRES(
                sizeof...(BlockedRanges) == N &&
                const_algorithm::and_c<
                    concept::blocked_range<BlockedRanges>()...>() &&
                std::is_constructible<
                    detail::blocked_range::BlockedRangeTuple<N, Extents...>,
                    const BlockedRanges&...>::value)>
  KBlockedRange(const BlockedRanges&... blocked_ranges)
      : _blocked_ranges(blocked_ranges...) {}

  template <int I, CONCEPT_REQUIRES(I >= 0 && I < N)>
  const auto& project() const {
    return std::get<I>(_blocked_ranges);
  }

  bool empty() const {
    return detail::blocked_range::is_empty(_blocked_ranges);
  }
  bool is_divisible() const {
    return detail::blocked_range::is_divisible(_blocked_ranges);
  }

 private:
  template <std::size_t... Indexes, class... ExtentsInit>
  KBlockedRange(std::index_sequence<Indexes...>, ExtentsInit... extents)
      : _blocked_ranges(BlockedRange<
            detail::blocked_range::ExtentType<Indexes, Extents...>>(
            detail::blocked_range::get_first<Indexes>(extents...),
            detail::blocked_range::get_last<Indexes>(extents...))...) {}
  detail::blocked_range::BlockedRangeTuple<N, Extents...> _blocked_ranges;
};

template <
    int I, int N, class... Extents,
    CONCEPT_REQUIRES(I >= 0 && I < N &&
                     const_algorithm::and_c<concept::extent<Extents>()...>())>
const auto& project(const KBlockedRange<N, Extents...>& k_blocked_range) {
  return k_blocked_range.template project<I>();
}

CONCEPT_ASSERT(concept::blocked_range<KBlockedRange<1, std::size_t>>());

////////////////////////
// make_blocked_range //
////////////////////////

template <class Extent, CONCEPT_REQUIRES(concept::extent<Extent>())>
auto make_blocked_range(Extent first, Extent last, std::size_t grainsize = 1) {
  return BlockedRange<Extent>(first, last, grainsize);
}

namespace detail {
namespace blocked_range {
template <
    std::size_t... Indexes, class... Extents,
    CONCEPT_REQUIRES(sizeof...(Indexes)*2 == sizeof...(Extents) &&
                     const_algorithm::and_c<concept::extent<Extents>()...>())>
auto make_blocked_range_impl(std::index_sequence<Indexes...>,
                             Extents... extents) {
  return KBlockedRange<sizeof...(Indexes),
                       decltype(get_first<Indexes>(extents...))...>(extents...);
}
}
}

template <
    class... Extents,
    CONCEPT_REQUIRES(const_algorithm::and_c<concept::extent<Extents>()...>() &&
                     sizeof...(Extents) % 2 == 0)>
auto make_blocked_range(Extents... extents) {
  return detail::blocked_range::make_blocked_range_impl(
      std::make_index_sequence<sizeof...(Extents) / 2>(), extents...);
}

template <
    class... Extents,
    CONCEPT_REQUIRES(sizeof...(Extents) > 1 &&
                     const_algorithm::and_c<concept::extent<Extents>()...>())>
auto make_blocked_range(const BlockedRange<Extents>&... blocked_ranges) {
  return KBlockedRange<sizeof...(Extents), Extents...>(blocked_ranges...);
}
}
}
}
