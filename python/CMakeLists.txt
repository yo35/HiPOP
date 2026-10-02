nanobind_add_module(cpp
    hipop/cpp/main.cpp
    hipop/cpp/graph.cpp
    hipop/cpp/shortest_path.cpp
)
target_link_libraries(cpp PRIVATE hipoplib)

# Only generate the type stubs and install the extension when the build is driven by scikit-build-core.
if(DEFINED SKBUILD)

    nanobind_add_stub(cpp_stubs
        RECURSIVE
        MODULE cpp
        PYTHON_PATH $<TARGET_FILE_DIR:cpp>
        OUTPUT_PATH stubs/hipop
        OUTPUT
            stubs/hipop/cpp/__init__.pyi
            stubs/hipop/cpp/graph.pyi
            stubs/hipop/cpp/shortest_path.pyi
        DEPENDS cpp
    )

    install(TARGETS cpp LIBRARY DESTINATION hipop)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/stubs/" DESTINATION .)

endif()
