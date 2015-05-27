#pragma once

#include <echo/memory.h>

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
  return memory::SimdAllocator<Allocator<T>>();
}
}
}
