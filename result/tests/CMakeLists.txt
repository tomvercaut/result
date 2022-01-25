find_package(Catch2 3.0.0 REQUIRED)

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
