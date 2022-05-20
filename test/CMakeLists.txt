# Bare-bones test definition. It only define the logical connection between
# components.
#
# This file does not assume any kind of package manager. It assumes dependencies
# will be found by find_package.
project(umbra_test)

include(early_hook.cmake)

add_executable(tester test.cpp)
find_package(Catch2 REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(tester PRIVATE umbra::umbra Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tester)

# How to test that compilation fails:
# https://cmake.org/pipermail/cmake/2017-September/066219.html
# pfultz2 at yahoo.com
# Wed Sep 6 23:39:37 EDT 2017
function(test_that_compile_errors name longname file regex)
  add_library(${name} STATIC EXCLUDE_FROM_ALL ${file})
  target_link_libraries(${name} PRIVATE umbra::umbra)
  add_test(
    NAME ${longname}
    COMMAND ${CMAKE_COMMAND} --build . --target ${name} --config
            $<CONFIGURATION>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  set_tests_properties(${longname} PROPERTIES PASS_REGULAR_EXPRESSION ${regex})
endfunction()

test_that_compile_errors(shadow_warning_on
  "Ensure that shadow warnings are enabled"
  shadow_warning_on.cpp "(shadows|hides)")

test_that_compile_errors(poison
  "Accessing a poisoned variable causes a warning"
  poison_test.cpp "poisoned by UMBRA_POISON")
