cmake_minimum_required(VERSION 2.6)
project(tddd16-lab3)

find_package(FLEX)
find_package(BISON)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -stdlib=libc++")
add_subdirectory(googletest)
enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/include ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

flex_target(scanner
  lib/scanner.l
  ${CMAKE_BINARY_DIR}/scanner.l.cc
)

bison_target(parser
  lib/parser.y
  ${CMAKE_BINARY_DIR}/parser.y.cc
)

add_flex_bison_dependency(scanner parser)

add_executable(scanner
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  lib/ast.cc
  lib/codegen.cc
  lib/main.cc
  lib/string.cc
  lib/symtab.cc
  lib/main.cc
)

