find_package(Catch2 3.0.0 QUIET)

if (NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2
            GIT_TAG v3.0.0-preview4
    )
    FetchContent_MakeAvailable(Catch2)
    message(STATUS "Catch2 available in: ${Catch2_SOURCE_DIR}")
    list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
endif()

add_executable(result_test
        src/main.cpp
        src/result.cpp
        )
add_dependencies(result_test result::result)
target_include_directories(result_test
        PRIVATE
            ${result_SOURCE_DIR}/include/
        )
target_link_libraries(result_test
        PRIVATE
            Catch2::Catch2
        )
target_link_libraries(result_test
        INTERFACE
            result::result
        )

include(CTest)
include(Catch)
catch_discover_tests(result_test)
