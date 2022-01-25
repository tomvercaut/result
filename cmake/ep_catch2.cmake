include(ExternalProject)

message("External project - Catch2")
if (NOT DEFINED ExternalInstallDir)
    message(FATAL_ERROR "Variable ExternalInstallDir is not defined")
endif ()
ExternalProject_Add(ep_catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.0.0-preview4
    INSTALL_DIR ${ExternalInstallDir}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
)
ExternalProject_Get_Property(ep_catch2 install_dir)
set(EDEP_CATCH2_INSTALL_DIR ${install_dir} CACHE INTERNAL "External Catch2 installation directory")
