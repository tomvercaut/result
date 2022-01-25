include(ExternalProject)

set(ExternalInstallDir ${CMAKE_BINARY_DIR}/install CACHE INTERNAL
        "Directory in which the external dependencies are installed.")

option(RESULT_BUILD_TESTS "Build tests for the result project" ON)

set(DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)
list(APPEND EXTRA_CMAKE_ARGS
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH:PATH=${ExternalInstallDir}
        )

if (NOT RESULT_BUILD_TESTS)
    list(APPEND EXTRA_CMAKE_ARGS -DRESULT_BUILD_TESTS:BOOL=OFF)
else()
    list(APPEND EXTRA_CMAKE_ARGS -DRESULT_BUILD_TESTS:BOOL=ON)

    # Catch2
    option(USE_SUPERBUILD_CATCH2 "Download and compile the Catch2 library as an external dependency." ON)
    if (USE_SUPERBUILD_CATCH2)
        include(${CMAKE_SOURCE_DIR}/cmake/ep_catch2.cmake)
        list(APPEND DEPENDENCIES ep_catch2)
    endif ()
endif()

# result
ExternalProject_Add(ep_result
        DEPENDS ${DEPENDENCIES}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/result
        INSTALL_DIR ${ExternalInstallDir}
        TEST_BEFORE_INSTALL RESULT_BUILD_TESTS
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
            -DUSE_SUPERBUILD_RESULT:BOOL=OFF
            ${EXTRA_CMAKE_ARGS}
        )
ExternalProject_Get_Property(ep_result install_dir)
set(EDEP_RESULT_TOOLS_INSTALL_DIR ${install_dir} CACHE INTERNAL "External result installation directory")
