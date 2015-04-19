cmake_minimum_required(VERSION 3.0)

project("echo-execution_context")

enable_testing()

include(Echo/Test)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/opt/intel/mkl/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DMKL_ILP64 -openmp -std=c++14 -march=native")

file(GLOB headers
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/*.h"
)

file(GLOB headers-execution_context
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/execution_context/*.h"
)

file(GLOB unittests
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/unittest/*_test.cpp"
)


install(FILES ${headers} DESTINATION include/echo)
install(FILES ${headers-execution_context} DESTINATION include/echo/execution_context)

add_catch_test(execution_context-test ${unittests})

target_link_libraries(execution_context-test tbb)
target_link_libraries(execution_context-test /opt/intel/mkl/lib/intel64/libmkl_intel_ilp64.so 
                                  /opt/intel/mkl/lib/intel64/libmkl_core.so
                                  /opt/intel/mkl/lib/intel64/libmkl_intel_thread.so 
                                  /opt/intel/lib/intel64/libiomp5.so
                                  pthread 
                                  m)
