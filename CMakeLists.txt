cmake_minimum_required(VERSION 3.0)

project("EchoExecutionContext")

enable_testing()

option(ECHO_INTEL_EXECUTION_CONTEXT "support intel execution context" ON)

find_package(Echo)
include(Echo/Default)

if(ECHO_BUILD_TESTS OR ECHO_BUILD_BENCHMARKS)
  if(NOT ECHO_INTEL_EXECUTION_CONTEXT)
    message(FATAL_ERROR "intel execution context is required to build tests or \
benchmarks")
  endif()
endif()


set(ECHOEXECUTIONCONTEXT_MAJOR_VERSION 0)
set(ECHOEXECUTIONCONTEXT_MINOR_VERSION 0)
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

find_package( TBB MODULE REQUIRED)
find_package( MKL MODULE REQUIRED)

include_directories(SYSTEM ${TBB_INCLUDE_DIRS})
include_directories(SYSTEM ${MKL_INCLUDE_DIRS})

configure_file(config/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfig.cmake" @ONLY)
configure_file(config/EchoExecutionContextConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfigVersion.cmake" @ONLY)

configure_file(cmake/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/EchoExecutionContextConfig.cmake" @ONLY)

ECHO_ADD_DEFAULT_TEST()

ECHO_ADD_DEFAULT_BENCHMARK()
ECHO_INSTALL_DEFAULT_HEADERS()

ECHO_TARGET_LINK_LIBRARIES(${TBB_LIBRARIES} ${MKL_LIBRARIES})

install(FILES 
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfig.cmake"
  "${PROJECT_BINARY_DIR}/config/EchoExecutionContextConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}") 
