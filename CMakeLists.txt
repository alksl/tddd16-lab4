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

add_test(
  NAME nested_function
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/nested_function)

add_test(
  NAME function_without_declaration
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/function_without_declaration)

add_test(
  NAME addition
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/addition)

add_test(
  NAME subtraction
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/subtraction)

add_test(
  NAME multiplication
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/multiplication)

add_test(
  NAME division
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/division)

add_test(
  NAME exponentiation
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/exponentiation)

add_test(
  NAME negation
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/negation)

add_test(
  NAME integer
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/integer)

add_test(
  NAME real
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/real)

add_test(
  NAME identifier
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/identifier)

add_test(
  NAME array_ref
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/array_ref)

add_test(
  NAME coerced_expression
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/coerced_expression)

add_test(
  NAME complex_expression
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/expressions/complex_expression)

add_test(
  NAME greater_or_equal
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/greater_or_equal)

add_test(
  NAME less_or_equal
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/less_or_equal)

add_test(
  NAME greater
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/greater)

add_test(
  NAME less
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/less)

add_test(
  NAME equal
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/equal)

add_test(
  NAME not_equal
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/not_equal)

add_test(
  NAME and
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/and)

add_test(
  NAME or
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/or)

add_test(
  NAME not
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/not)

add_test(
  NAME complex_condition
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/conditions/complex_condition)

add_test(
  NAME array_assignment
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/array_assignment)

add_test(
  NAME else_if
  COMMAND ${CMAKE_BINARY_DIR}/parser ${CMAKE_SOURCE_DIR}/test/else_if)

set_tests_properties(
  empty_function
  recursive_function
  nested_function
  function_without_declaration
  addition
  subtraction
  multiplication
  division
  exponentiation
  negation
  integer
  real
  identifier
  array_ref
  coerced_expression
  complex_expression
  greater_or_equal
  less_or_equal
  greater
  less
  equal
  not_equal
  and
  or
  not
  complex_condition
  array_assignment
  else_if
  PROPERTIES FAIL_REGULAR_EXPRESSION "Error")

