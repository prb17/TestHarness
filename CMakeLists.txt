cmake_minimum_required(VERSION 3.17.5)
project(TestHarness)
include(FetchContent)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dev")
set(CMAKE_BUILD_TYPE Debug)
set(TestHarness_VERSION "0.0.0")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/find-cmakes")

find_package(Zmq REQUIRED)
include_directories(${Zmq_INCLUDE_DIRS})
if(NOT Zmq_LIBRARIES AND Zmq_FOUND)
    link_directories(${Zmq_LIBRARIES_DIR})
endif(NOT Zmq_LIBRARIES AND Zmq_FOUND)

find_package(Czmq REQUIRED)
include_directories(${Czmq_INCLUDE_DIRS})
if(NOT Czmq_ROOT)
    link_directories(${Czmq_LIBRARIES_DIR})
endif(NOT Czmq_ROOT)

find_package(Utils REQUIRED)
include_directories(${Utils_INCLUDE_DIRS})

find_package(SimpleLogger REQUIRED)
include_directories(${SimpleLogger_INCLUDE_DIRS})

set(TestHarness_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${TestHarness_INCLUDE_DIRS})

set(TestHarness_Tests_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
include_directories(${TestHarness_Tests_DIRS})

add_subdirectory("src")
