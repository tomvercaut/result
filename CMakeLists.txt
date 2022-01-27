cmake_minimum_required(VERSION 3.21.1)
project(result
        LANGUAGES CXX
        VERSION 0.0.1
        )

set(CMAKE_CXX_STANDARD 20)
option(RESULT_BUILD_TESTS "Build tests for the result project" ON)
option(RESULT_BUILD_EXAMPLES "Build examples for the result project" ON)
option(RESULT_GENERATE_DOC "Build documentation for the result project" OFF)

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

if (WIN32)
    set(cmake_windows_export_all_symbols 1)
endif ()

add_library(result INTERFACE include/result/result.hpp)
add_library(result::result ALIAS result)

install(TARGETS result
        EXPORT result-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
install(DIRECTORY include/result
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
        )
install(EXPORT result-targets
        FILE result-targets.cmake
        NAMESPACE result::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/result
        )

configure_package_config_file(
        cmake/result-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/result-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/result
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/result-config-version.cmake
        VERSION ${result_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/result-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/result-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/result
)

export(EXPORT result-targets
        FILE ${CMAKE_BINARY_DIR}/cmake/result-targets.cmake
        NAMESPACE result::
        )

if (RESULT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (RESULT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (RESULT_GENERATE_DOC)
    include(result/cmake/doxygen.cmake)
endif ()