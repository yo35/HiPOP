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
