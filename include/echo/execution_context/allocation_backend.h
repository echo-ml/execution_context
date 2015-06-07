#pragma once

#include <echo/memory.h>
#include <boost/align/aligned_allocator_adaptor.hpp>

namespace echo {
namespace execution_context {

template <template <class> class Allocator>
struct AllocationBackend {};

////////////////////
// make_allocator //
////////////////////

template <class T, template <class> class Allocator>
auto make_allocator(const AllocationBackend<Allocator>&) {
  return Allocator<T>();
}

////////////////////////////
// make_aligned_allocator //
////////////////////////////

template <class T, template <class> class Allocator>
auto make_aligned_allocator(const AllocationBackend<Allocator>&) {
  return boost::alignment::aligned_allocator_adaptor<Allocator<T>, ECHO_SIMD_ALIGNMENT>();
}
}
}
