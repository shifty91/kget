cmake_minimum_required(VERSION 3.2)
project(get)

set(SRCS
  src/main.cc
  src/tcp_connection.cc
  src/tcp_ssl_connection.cc
  src/url_parser.cc
  src/ftp.cc
  src/sftp.cc
  src/progress_bar.cc
  src/base64.cc
  src/protocol_dispatcher.cc
  src/config.cc
  src/connection.cc
)

set(VERSION "1.13")

# check for C++17
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" HAVE_CPP17)
if (NOT HAVE_CPP17)
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
endif()

# Set custom RPATH
set(CMAKE_INSTALL_RPATH "${CUSTOM_RPATH}")

# Add binary
add_executable(get ${SRCS})

# Setup compile options
target_compile_options(get PUBLIC -std=c++17 -pedantic -Wall -march=native)

find_package(PkgConfig REQUIRED)

# Search OpenSSL
pkg_search_module(OPENSSL openssl>=1.0.2)
if (OPENSSL_FOUND)
  target_include_directories(get PRIVATE ${OPENSSL_INCLUDE_DIRS})
  target_link_libraries(get ${OPENSSL_LIBRARIES})
  link_directories(${OPENSSL_LIBRARY_DIRS})
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
  set(HAVE_OPENSSL ON CACHE BOOL "Use OpenSSL")
endif()

# Search libssh2
pkg_search_module(LIBSSH2 libssh2)
if (LIBSSH2_FOUND)
  target_include_directories(get PRIVATE ${LIBSSH2_INCLUDE_DIRS})
  target_link_libraries(get ${LIBSSH2_LIBRARIES})
  link_directories(${LIBSSH2_LIBRARY_DIRS})
  message(STATUS "Using LibSSH2 ${LIBSSH2_VERSION}")
  set(HAVE_LIBSSH ON CACHE BOOL "Use LibSSH2")
endif()

# Search for libunwind
pkg_search_module(LIBUNWIND libunwind)
if (LIBUNWIND_FOUND)
  target_include_directories(get PRIVATE ${LIBUNWIND_INCLUDE_DIRS})
  target_link_libraries(get ${LIBUNWIND_LIBRARIES})
  link_directories(${LIBUNWIND_LIBRARY_DIRS})
  message(STATUS "Using Libunwind ${LIBUNWIND_VERSION}")
  set(HAVE_LIBUNWIND ON CACHE BOOL "Use Libunwind")
endif()

# Termios.h
include(CheckIncludeFiles)
check_include_files("termios.h" HAVE_TERMIOS_H)
if (NOT HAVE_TERMIOS_H)
  message(FATAL_ERROR "termios.h not found")
endif()

# config file
configure_file(
  "${PROJECT_SOURCE_DIR}/get_config.in"
  "${PROJECT_BINARY_DIR}/get_config.h"
  )
target_include_directories(get PRIVATE "${PROJECT_BINARY_DIR}")

# kopt
include(GNUInstallDirs)
include(ExternalProject)
externalproject_add(kopt
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib/kopt/"
  CMAKE_ARGS
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/kopt
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
add_library(kopt_lib STATIC IMPORTED)
set_target_properties(kopt_lib PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/kopt/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}kopt${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(KOPT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/kopt/include/")
add_dependencies(kopt_lib kopt)

target_include_directories(get PRIVATE "src" "src/ssl" "src/ssh" ${KOPT_INCLUDE_DIR})
target_link_libraries(get kopt_lib)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()
install(TARGETS get DESTINATION bin COMPONENT binaries)
