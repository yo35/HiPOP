if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64") # TODO keep or remove?
    include_directories(/opt/homebrew/opt/libomp/include)
    link_directories(/opt/homebrew/opt/libomp/lib)
endif()

pybind11_add_module(cpp # TODO rename module?
    hipop/cpp/main.cpp
    hipop/cpp/graph.cpp
    hipop/cpp/shortest_path.cpp
)
target_link_libraries(cpp PUBLIC hipoplib)
