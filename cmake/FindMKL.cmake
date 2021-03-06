# - Find Intel MKL
# Find the MKL libraries
#
# Options:
#
#   MKL_STATAIC       :   use static linking
#   MKL_MULTI_THREADED:   use multi-threading
#   MKL_SDL           :   Single Dynamic Library interface
#
# This module defines the following variables:
#
#   MKL_FOUND            : True if MKL_INCLUDE_DIR are found
#   MKL_INCLUDE_DIR      : where to find mkl.h, etc.
#   MKL_INCLUDE_DIRS     : set when MKL_INCLUDE_DIR found
#   MKL_LIBRARIES        : the library to link against.


include(FindPackageHandleStandardArgs)

find_package (Threads)

# set(INTEL_ROOT "/opt/intel" CACHE PATH "Folder contains intel libs")
# set(MKL_ROOT $ENV{MKLROOT} CACHE PATH "Folder contains MKL")

# set(INTEL_ROOT /opt/intel/compilers_and_libraries_2016.0.042/linux/)
set(INTEL_ROOT /opt/intel/compilers_and_libraries_2016.0.069/linux/compiler/)
# set(MKL_ROOT ${INTEL_ROOT}/mkl/)
set(MKL_ROOT /opt/intel/compilers_and_libraries_2016.0.069/linux/mkl/)

set(MKL_FOUND true)
set(MKL_INCLUDE_DIRS ${MKL_ROOT}/include)

find_library(MKL_INTERFACE_LIBRARY NAMES libmkl_intel_ilp64.a HINTS ${MKL_ROOT}/lib/intel64_lin)
find_library(MKL_CORE_LIBRARY NAMES libmkl_core.a HINTS ${MKL_ROOT}/lib/intel64_lin)
find_library(MKL_THREADING_LIBRARY NAMES libmkl_intel_thread.a HINTS ${MKL_ROOT}/lib/intel64_lin)
find_library(MKL_IOMP_LIBRARY NAMES libiomp5.a HINTS ${INTEL_ROOT}/lib/intel64_lin)
set(MKL_LIBRARIES -Wl,--start-group ${MKL_INTERFACE_LIBRARY} ${MKL_CORE_LIBRARY} 
  ${MKL_THREADING_LIBRARY} ${MKL_IOMP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} 
  -Wl,--end-group m dl)

# set(MKL_LIBRARIES  ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a
#                    ${MKLROOT}/lib/intel64/libmkl_core.a
#                    ${MKLROOT}/lib/intel64/libmkl_intel_thread.a
#                    pthread m)
set(MKL_CXXFLAGS "-DMKL_ILP64 -openmp")

# Find include dir
# find_path(MKL_INCLUDE_DIR mkl.h PATHS ${MKL_ROOT}/include)
# set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIRS} ${MKL_INCLUDE_DIR})

# Find include directory
#  There is no include folder under linux
# if(WIN32)
#     find_path(INTEL_INCLUDE_DIR omp.h
#         PATHS ${INTEL_ROOT}/include)
#     set(MKL_INCLUDE_DIR ${MKL_INCLUDE_DIR} ${INTEL_INCLUDE_DIR})
# endif()
#
# # Find libraries
#
# # Handle suffix
# set(_MKL_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
#
# if(WIN32)
#     if(MKL_STATAIC)
#         set(CMAKE_FIND_LIBRARY_SUFFIXES .lib)
#     else()
#         set(CMAKE_FIND_LIBRARY_SUFFIXES _dll.lib)
#     endif()
# else()
#     if(MKL_STATAIC)
#         set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
#     else()
#         set(CMAKE_FIND_LIBRARY_SUFFIXES .so)
#     endif()
# endif()
#
#
# # MKL is composed by four layers: Interface, Threading, Computational and RTL
#
# if(MKL_SDL)
#     find_library(MKL_LIBRARY mkl_rt
#         PATHS ${MKL_ROOT}/lib/ia32/)
#
#     set(MKL_MINIMAL_LIBRARY ${MKL_LIBRARY})
# else()
#     ######################### Interface layer #######################
#     if(WIN32)
#         set(MKL_INTERFACE_LIBNAME mkl_intel_c)
#     else()
#         set(MKL_INTERFACE_LIBNAME mkl_intel)
#     endif()
#
#     find_library(MKL_INTERFACE_LIBRARY ${MKL_INTERFACE_LIBNAME}
#         PATHS ${MKL_ROOT}/lib/ia32/)
#
#     ######################## Threading layer ########################
#     if(MKL_MULTI_THREADED)
#         set(MKL_THREADING_LIBNAME mkl_intel_thread)
#     else()
#         set(MKL_THREADING_LIBNAME mkl_sequential)
#     endif()
#
#     find_library(MKL_THREADING_LIBRARY ${MKL_THREADING_LIBNAME}
#         PATHS ${MKL_ROOT}/lib/ia32/)
#
#     ####################### Computational layer #####################
#     find_library(MKL_CORE_LIBRARY mkl_core
#         PATHS ${MKL_ROOT}/lib/ia32/)
#     find_library(MKL_FFT_LIBRARY mkl_cdft_core
#         PATHS ${MKL_ROOT}/lib/ia32/)
#     find_library(MKL_SCALAPACK_LIBRARY mkl_scalapack_core
#         PATHS ${MKL_ROOT}/lib/ia32/)
#
#     ############################ RTL layer ##########################
    # if(WIN32)
    #     set(MKL_RTL_LIBNAME libiomp5md)
    # else()
    #     set(MKL_RTL_LIBNAME libiomp5)
    # endif()
    # find_library(MKL_RTL_LIBRARY ${MKL_RTL_LIBNAME}
    #     PATHS ${INTEL_RTL_ROOT}/lib)
#
    # set(MKL_LIBRARY ${MKL_INTERFACE_LIBRARY} ${MKL_THREADING_LIBRARY} ${MKL_CORE_LIBRARY} ${MKL_FFT_LIBRARY} ${MKL_SCALAPACK_LIBRARY} ${MKL_RTL_LIBRARY})
#     set(MKL_MINIMAL_LIBRARY ${MKL_INTERFACE_LIBRARY} ${MKL_THREADING_LIBRARY} ${MKL_CORE_LIBRARY} ${MKL_RTL_LIBRARY})
# endif()
#
# set(CMAKE_FIND_LIBRARY_SUFFIXES ${_MKL_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
#
# find_package_handle_standard_args(MKL DEFAULT_MSG
#     MKL_INCLUDE_DIR MKL_LIBRARY MKL_MINIMAL_LIBRARY)
#
# if(MKL_FOUND)
#     set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR})
    # set(MKL_LIBRARIES ${MKL_LIBRARY})
    # set(MKL_MINIMAL_LIBRARIES ${MKL_LIBRARY})
# endif()
