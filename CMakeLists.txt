cmake_minimum_required(VERSION 3.19)

file(STRINGS VERSION.txt HIPOP_VERSION)
project(HiPOP
    VERSION "${HIPOP_VERSION}"
    DESCRIPTION "High Performance Optimal Path module"
    HOMEPAGE_URL "https://github.com/EMob-Lab/HiPOP"
    LANGUAGES CXX
)

option(HIPOP_WITH_TESTS "Compile HiPOP tests (for core library)" ON)
option(HIPOP_WITH_EXAMPLES "Compile HiPOP examples (for core library)" ON)

# Default build configuration.
# Can be overriden with `-DCMAKE_BUILD_TYPE=<Debug|RelWithDebInfo|Release|...>` when calling CMake,
# or by using an environment variable `export CMAKE_BUILD_TYPE=...`.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "CMAKE_BUILD_TYPE set to [${CMAKE_BUILD_TYPE}]")

# Default C++ standard for all the targets in the project.
# Can be overriden with `-DCMAKE_CXX_STANDARD=...` when calling CMake (but should be at least the default value).
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
message(STATUS "CMAKE_CXX_STANDARD set to [${CMAKE_CXX_STANDARD}]")

# 3rd-party dependencies.
find_package(OpenMP REQUIRED)

# Enable CTest and related testing features if requested
# (must be done before adding the subdirectories).
if(HIPOP_WITH_TESTS)
    enable_testing()
endif()

# `cpp` subdirectory -> C++ library with related tests and examples, excluding the Python wrapper.
message(STATUS "Building HiPOP core library")
add_subdirectory(cpp)
