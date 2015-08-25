cmake_minimum_required(VERSION 2.6)
project(get)

set(SRCS
  main.cc
  tcp_connection.cc
  tcp_ssl_connection.cc
  url_parser.cc
  ftp.cc
  sftp.cc
  progress_bar.cc
  base64.cc
  protocol_dispatcher.cc
  config.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pedantic -Wall -march=native")
set(CMAKE_BUILD_TYPE "Release")

# Search OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

# libssh2
pkg_search_module(LIBSSH2 REQUIRED libssh2)
if (LIBSSH2_FOUND)
  include_directories(${LIBSSH2_INCLUDE_DIRS})
  message(STATUS "Using LibSSH2 ${LIBSSH2_VERSION}")
endif()

# Termios.h
INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES("termios.h" HAVE_TERMIOS_H)
if (NOT HAVE_TERMIOS_H)
  message(FATAL_ERROR "termios.h not found")
endif()

add_executable(get ${SRCS})
target_link_libraries(get ${OPENSSL_LIBRARIES})
target_link_libraries(get ${LIBSSH2_LIBRARIES})
install(TARGETS get DESTINATION /usr/bin COMPONENT binaries)
