# TODO to remove (config achieved in parent dir)

set(HIPOP_SRC
  create.cpp
  graph.cpp
  shortest_path.cpp
)

add_library(hipoplib ${HIPOP_SRC})
set_target_properties(hipoplib PROPERTIES POSITION_INDEPENDENT_CODE ON)  # Needed to link the hipop static lib to the pybind dynamic one

target_link_libraries(hipoplib PUBLIC OpenMP::OpenMP_CXX)

if(APPLE)
    target_link_directories(hipoplib PRIVATE /opt/homebrew/opt/libomp/lib)
endif()

set_property(TARGET hipoplib PROPERTY CXX_STANDARD 17)

target_include_directories( hipoplib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Link OpenMP
target_link_libraries(hipoplib PUBLIC OpenMP::OpenMP_CXX)

# Specific case for macOS: force the correct flags for AppleClang
if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
  message(STATUS "Configuring hipoplib for macOS with OpenMP")

  # Required to compile with OpenMP using AppleClang
  target_compile_options(hipoplib PRIVATE -Xpreprocessor -fopenmp)

  # Explicitly link the OpenMP library
  target_link_libraries(hipoplib PRIVATE omp)

  # Specifies where to look for libomp.dylib
  target_link_directories(hipoplib PRIVATE /opt/homebrew/opt/libomp/lib)
endif()