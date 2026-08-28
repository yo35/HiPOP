pybind11_add_module(cpp # TODO rename module?
    hipop/cpp/main.cpp
    hipop/cpp/graph.cpp
    hipop/cpp/shortest_path.cpp
)
target_link_libraries(cpp PRIVATE hipoplib)

# Only install the extension when the build is driven by scikit-build-core.
if(DEFINED SKBUILD)
    install(TARGETS cpp LIBRARY DESTINATION hipop)
endif()

# TODO add tests for the Python wrapper
# TODO impl build of examples
