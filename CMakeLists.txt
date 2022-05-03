cmake_minimum_required(VERSION 3.20)

project(
  umbra
  VERSION 0.0.0
  DESCRIPTION "Macro utilities for shadowing"
  LANGUAGES CXX)

include(early_hook.cmake)

find_package(Boost REQUIRED)

add_library(umbra INTERFACE)
add_library(umbra::umbra ALIAS umbra)
target_include_directories(umbra INTERFACE include)
target_compile_features(umbra INTERFACE cxx_std_20)

add_subdirectory(test)
