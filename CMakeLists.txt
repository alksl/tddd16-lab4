cmake_minimum_required(VERSION 2.6)
project(tddd16-lab3)

find_package(FLEX)
find_package(BISON)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -stdlib=libc++")
enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/include)

flex_target(scanner
  lib/scanner.l
  ${CMAKE_BINARY_DIR}/scanner.l.cc
)

bison_target(parser
  lib/parser.y
  ${CMAKE_BINARY_DIR}/parser.y.cc
)

add_flex_bison_dependency(scanner parser)

add_executable(parser
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  lib/ast.cc
  lib/codegen.cc
  lib/main.cc
  lib/string.cc
  lib/symtab.cc
  lib/main.cc
)

add_test(
  NAME empty_function
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/empty_function)

add_test(
  NAME recursive_function
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/recursive_function)

set_tests_properties(
  empty_function
  recursive_function
  PROPERTIES FAIL_REGULAR_EXPRESSION "Error")

