# Bare-bones test definition.
# It only define the logical connection between components.
#
# This file does not assume any kind of package manager.
# It assumes dependencies will be found by find_package.
project(umbra_test)

include(early_hook.cmake)

add_executable(tester test.cpp)
find_package(Catch2 REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(tester
  PRIVATE
  umbra::umbra
  Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tester)

add_library(poison STATIC EXCLUDE_FROM_ALL poison_test.cpp)
target_link_libraries(poison PRIVATE umbra::umbra)
add_test(NAME poison
  COMMAND ${CMAKE_COMMAND} --build . --target poison --config $<CONFIGURATION>
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(poison PROPERTIES PASS_REGULAR_EXPRESSION "poisoned by UMBRA_POISON")
