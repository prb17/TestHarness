cmake_minimum_required(VERSION 3.17.5)
project(TestHarness)
include(FetchContent)
set(CMAKE_BUILD_TYPE Debug)
set(TestHarness_VERSION "0.0.0")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/find-cmakes")

find_package(Utils REQUIRED)
include_directories(${Utils_INCLUDE_DIRS})

find_package(SimpleLogger REQUIRED)
include_directories(${SimpleLogger_INCLUDE_DIRS})

set(TestHarness_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${TestHarness_INCLUDE_DIRS})

add_subdirectory("src")