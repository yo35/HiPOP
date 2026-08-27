pybind11_add_module(cpp # TODO rename module?
    hipop/cpp/main.cpp
    hipop/cpp/graph.cpp
    hipop/cpp/shortest_path.cpp
)
target_link_libraries(cpp PUBLIC hipoplib)
