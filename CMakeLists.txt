cmake_minimum_required(VERSION 2.6)
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
  src/net_utils.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pedantic -Wall -march=native")
set(CMAKE_BUILD_TYPE "Release")

# check for C++14
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if (NOT COMPILER_SUPPORTS_CXX14)
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++14 support.")
endif()

# Search OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl>=1.0.2)

if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  link_directories(${OPENSSL_LIBRARY_DIRS})
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

# libssh2
pkg_search_module(LIBSSH2 REQUIRED libssh2)
if (LIBSSH2_FOUND)
  include_directories(${LIBSSH2_INCLUDE_DIRS})
  link_directories(${LIBSSH2_LIBRARY_DIRS})
  message(STATUS "Using LibSSH2 ${LIBSSH2_VERSION}")
endif()

# Termios.h
include(CheckIncludeFiles)
check_include_files("termios.h" HAVE_TERMIOS_H)
if (NOT HAVE_TERMIOS_H)
  message(FATAL_ERROR "termios.h not found")
endif()

include_directories("src" "src/ssl" "src/ssh")
add_executable(get ${SRCS})
target_link_libraries(get ${OPENSSL_LIBRARIES})
target_link_libraries(get ${LIBSSH2_LIBRARIES})
install(TARGETS get DESTINATION /usr/bin COMPONENT binaries)
