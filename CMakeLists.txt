cmake_minimum_required(VERSION 3.0)

project("EchoExecutionContext")

set(ECHOEXECUTIONCONTEXT_MAJOR_VERSION 0)
set(ECHOEXECUTIONCONTEXT_MINOR_VERSION 1)
set(ECHOEXECUTIONCONTEXT_PATCH_VERSION 0)
set(ECHOEXECUTIONCONTEXT_VERSION
    "${ECHOEXECUTIONCONTEXT_MAJOR_VERSION}."
    "${ECHOEXECUTIONCONTEXT_MINOR_VERSION}."
    "${ECHOEXECUTIONCONTEXT_PATCH_VERSION}")

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/EchoExecutionContext)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

enable_testing()

find_package( TBB MODULE REQUIRED)
find_package( MKL MODULE REQUIRED)

include(Echo/Test)
include(Echo/Benchmark)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/opt/intel/mkl/include)

include_directories(${TBB_INCLUDE_DIRS})
include_directories(${MKL_INCLUDE_DIRS})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${MKL_CXXFLAGS} -std=c++14 -march=native")

file(GLOB headers
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/*.h"
)

file(GLOB headers-execution_context
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/execution_context/*.h"
)

file(GLOB headers-execution_context-tbb
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/execution_context/tbb/*.h"
)

file(GLOB headers-execution_context-mkl
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/execution_context/mkl/*.h"
)

file(GLOB unittests
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/unittest/*_test.cpp"
)

file(GLOB benchmarks
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*_benchmark.cpp"
)


add_catch_test(t ${unittests})

target_link_libraries(t ${TBB_LIBRARIES})
target_link_libraries(t ${MKL_LIBRARIES})

add_benchmark(benchmark ${benchmarks})
target_link_libraries(benchmark ${TBB_LIBRARIES})
target_link_libraries(benchmark ${MKL_LIBRARIES})


configure_file(config/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfig.cmake" @ONLY)
configure_file(config/EchoExecutionContextConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfigVersion.cmake" @ONLY)

install(FILES ${headers} DESTINATION include/echo)
install(FILES ${headers-execution_context} DESTINATION include/echo/execution_context)
install(FILES ${headers-execution_context-tbb} DESTINATION
  include/echo/execution_context/tbb)
install(FILES ${headers-execution_context-mkl} DESTINATION
  include/echo/execution_context/mkl)


configure_file(cmake/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/EchoExecutionContextConfig.cmake" @ONLY)

install(FILES 
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfig.cmake"
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}") 
