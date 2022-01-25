add_executable(example_01
        src/example_01.cpp
        )
add_dependencies(example_01 result::result)
target_include_directories(example_01
        PRIVATE
            ${result_SOURCE_DIR}/include/
        )
target_link_libraries(example_01
        INTERFACE
            result::result
        )
