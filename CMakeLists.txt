cmake_minimum_required(VERSION 3.19)

project(HiPOP VERSION 0.0.3) # TODO project name/version set from top-level

option(HIPOP_WITH_PYTHON "Compile Python wrapper" ON)
option(HIPOP_WITH_TESTS "Compile HiPOP tests (for core library + Python wrapper if enabled)" ON)
option(HIPOP_WITH_EXAMPLES "Compile HiPOP examples (for core library + Python wrapper if enabled)" OFF) # TODO default on

find_package(OpenMP REQUIRED)

if (HIPOP_WITH_PYTHON)
    set(PYBIND11_FINDPYTHON ON) # See: https://pybind11.readthedocs.io/en/stable/cmake/#modes
    find_package(pybind11 REQUIRED)
endif()

# Default C++ standard for all the targets in the project.
# Can be overriden with `-DCMAKE_CXX_STANDARD=...` when calling CMake (but should be at least the default value).
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

# Enable CTest and related testing features if requested
# (must be done before adding the subdirectories).
if (HIPOP_WITH_TESTS)
    enable_testing()
endif()

# `cpp` subdirectory -> C++ library with related tests and examples, excluding the Python wrapper.
message(STATUS "Building HiPOP core library")
add_subdirectory(cpp)

# `python` subdirectory -> Python wrapper, with tests and examples specific to it.
if(HIPOP_WITH_PYTHON)
    message(STATUS "Building HiPOP Python wrapper")
    add_subdirectory(python)
endif()
