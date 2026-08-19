cmake_minimum_required(VERSION 3.21)
project(TaskManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts DBus Test)
qt_standard_project_setup()

option(TASKMANAGER_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src)

if(TASKMANAGER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
