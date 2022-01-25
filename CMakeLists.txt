cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

option(USE_SUPERBUILD_RESULT "Download and compile external dependencies, prior to building result" ON)

if (USE_SUPERBUILD_RESULT)
    project(result_superbuild)
    include(cmake/superbuild.cmake)
    return()
else()
    add_subdirectory(result)
    return()
endif ()
