from hipop.graph import OrientedGraph


# Same test scenario as in cpp/tests/testGraph.cpp.
# WARNING: any modification in this test should be reflected in the C++ test as well.
def test_graph() -> None:

    G = OrientedGraph()
    G.add_node("a", 0, 0, "")

    exclude_movements = {"a": {"c"}}
    G.add_node("b", 2, 5, "", exclude_movements)

    G.add_node("c", 12.0, 43.0, "")

    G.add_node("d", 435, 345, "")
    G.add_link("a_b", "a", "b", 12, {"PersonalVehicle": {"time": 12}}, "CAR")
    G.add_link("b_c", "b", "c", 12, {"PersonalVehicle": {"time": 12}}, "CAR")
    G.add_link("b_d", "b", "d", 12, {"PersonalVehicle": {"time": 12}}, "CAR")

    exits = G.nodes["b"].get_exits("a")

    assert len(exits) == 1, "Exits does not return one link"
    assert exits[0].downstream == "d", "Node should be d"
