set(Tests
    testCopyGraph.cpp
    testGraph.cpp
    testGraphMerge.cpp
    testDijkstra1.cpp
    testDijkstra2.cpp
    testKShortestPath.cpp
    testYenKShortestPath.cpp
    testManhattan.cpp
    testparallelKShortestPath.cpp
)

# Create a single executable (aka. the driver) capable of running each registered test.
create_test_sourcelist(CommonCxxTestsSrc CommonCxxTests.cpp ${Tests})
add_executable(CommonCxxTests ${CommonCxxTestsSrc})
target_link_libraries(CommonCxxTests hipoplib)
target_include_directories(CommonCxxTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add all the ADD_TEST for each test
foreach(test ${Tests})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND CommonCxxTests ${TName})
endforeach()

# Cas spécifique macOS : forcer les bons flags pour AppleClang TODO keep or remove?
if(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    message(STATUS "Configuring CommonCxxTests for macOS with OpenMP")

    # Nécessaire pour compiler avec OpenMP avec AppleClang
    target_compile_options(CommonCxxTests PRIVATE -Xpreprocessor -fopenmp)

    # Lier explicitement la bibliothèque OpenMP
    #target_link_libraries(CommonCxxTests PRIVATE omp)

    # Spécifie où chercher libomp.dylib
    target_link_directories(CommonCxxTests PRIVATE /opt/homebrew/opt/libomp/lib)
endif()
