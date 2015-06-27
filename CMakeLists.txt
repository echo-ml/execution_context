cmake_minimum_required(VERSION 3.0)

project("EchoExecutionContext")

enable_testing()

option(ECHO_INTEL_EXECUTION_CONTEXT "support intel execution context" ON)

find_package(Echo)
include(Echo/Default)

set(USE_TBB OFF)
set(USE_MKL OFF)

if(ECHO_BUILD_TESTS OR ECHO_BUILD_BENCHMARKS)
  if(NOT ECHO_INTEL_EXECUTION_CONTEXT)
    message(FATAL_ERROR "intel execution context is required to build tests or \
benchmarks")
  else()
    set(USE_TBB ON)
    set(USE_MKL ON)
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

set(EXECUTION_CONTEXT_LIBRARIES "")

if(USE_MKL)
  find_package( MKL MODULE REQUIRED)
  include_directories(SYSTEM ${MKL_INCLUDE_DIRS})
  list(APPEND EXECUTION_CONTEXT_LIBRARIES ${MKL_LIBRARIES})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MKL_CXXFLAGS}")
endif()
if(USE_TBB)
  find_package( TBB MODULE REQUIRED)
  include_directories(SYSTEM ${TBB_INCLUDE_DIRS})
  list(APPEND EXECUTION_CONTEXT_LIBRARIES ${TBB_LIBRARIES})
endif()

if(ECHO_INTEL_EXECUTION_CONTEXT)
  set(ECHO_INTEL_EXECUTION_CONTEXT_INCLUDES ${TBB_INCLUDE_DIRS} 
    ${MKL_INCLUDE_DIRS})
  set(ECHO_INTEL_EXECUTION_CONTEXT_LIBRARIES ${TBB_LIBRARIES}
    ${MKL_LIBRARIES})
  set(ECHO_INTEL_EXECUTION_CONTEXT_CXX_FLAGS ${MKL_CXXFLAGS})
endif()


configure_file(cmake/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/cmake/EchoExecutionContextConfig.cmake" @ONLY)
configure_file(cmake/EchoExecutionContextConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/cmake/EchoExecutionContextConfigVersion.cmake" @ONLY)

configure_file(cmake/EchoExecutionContextConfig.cmake.in
  "${PROJECT_BINARY_DIR}/EchoExecutionContextConfig.cmake" @ONLY)

ECHO_ADD_DEFAULT_TEST()

ECHO_ADD_DEFAULT_BENCHMARK()
ECHO_INSTALL_DEFAULT_HEADERS()

ECHO_TARGET_LINK_LIBRARIES(${EXECUTION_CONTEXT_LIBRARIES})
message("MKL LIBS=${EXECUTION_CONTEXT_LIBRARIES}")
message("MKL LIBS=${MKL_LIBRARIES}")

install(FILES 
  "${PROJECT_BINARY_DIR}/cmake/EchoExecutionContextConfig.cmake"
  "${PROJECT_BINARY_DIR}/cmake/EchoExecutionContextConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}") 
