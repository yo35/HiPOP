# Same test scenarios as in cpp/tests/testGraph.cpp, except for the parts
# that have no equivalent between the C++ and Python APIs.
# WARNING: any modification in this file should be reflected in the C++ test as well.

import pytest

from hipop.graph import OrientedGraph


#        10          20
#   A --------> B --------> C      D (isolated)
def _make_graph() -> OrientedGraph:

    G = OrientedGraph()

    G.add_node("A", 0, 0, "")
    G.add_node("B", 1, 0, "")
    G.add_node("C", 2, 0, "")
    G.add_node("D", 3, 3, "")

    G.add_link("A_B", "A", "B", 10, {"CAR": {"time": 1}}, "CarLayer")
    G.add_link("B_C", "B", "C", 20, {"BUS": {"time": 2}}, "BusLayer")

    return G


def test_empty_graph() -> None:

    G = OrientedGraph()

    assert G.node_count == 0, "node_count should be 0"
    assert G.link_count == 0, "link_count should be 0"
    assert "A" not in G.nodes, "`in G.nodes` should be false"
    assert "A_B" not in G.links, "`in G.links` should be false"
    assert G.find_node("A") is None, "find_node() should return None"
    assert G.find_link("A_B") is None, "find_link() should return None"
    with pytest.raises(ValueError):
        G.node("A")
    with pytest.raises(ValueError):
        G.link("A_B")

    assert len(G.nodes) == 0 and list(G.nodes) == [], "Node view should be empty"
    assert len(G.links) == 0 and list(G.links) == [], "Link view should be empty"


def test_node_accessors() -> None:

    G = _make_graph()

    assert G.node_count == 4, "node_count should be 4"
    assert "A" in G.nodes and "D" in G.nodes, "`in G.nodes` should be true for existing nodes"
    assert "Z" not in G.nodes, "`in G.nodes` should be false for unknown nodes"

    # node()
    a = G.node("A")
    assert a.id == "A" and a.position == [0, 0], "node() should return node 'A'"
    assert G.node("A") is a, "node() should always return the same node"
    with pytest.raises(ValueError):
        G.node("Z")

    # find_node()
    assert G.find_node("A") is a, "find_node() should return node 'A'"
    assert G.find_node("Z") is None, "find_node() should return None for unknown nodes"


def test_link_accessors() -> None:

    G = _make_graph()

    assert G.link_count == 2, "link_count should be 2"
    assert "A_B" in G.links and "B_C" in G.links, "`in G.links` should be true for existing links"
    assert "Z" not in G.links, "`in G.links` should be false for unknown links"

    # link()
    ab = G.link("A_B")
    assert ab.id == "A_B" and ab.label == "CarLayer" and ab.length == 10, "link() should return link 'A_B'"
    assert ab.upstream == "A" and ab.downstream == "B", "Link 'A_B' should go from 'A' to 'B'"
    assert G.link("A_B") is ab, "link() should always return the same link"
    with pytest.raises(ValueError):
        G.link("Z")

    # find_link()
    assert G.find_link("A_B") is ab, "find_link() should return link 'A_B'"
    assert G.find_link("Z") is None, "find_link() should return None for unknown links"

    # Modification through the accessors.
    G.link("A_B").update_costs({"CAR": {"time": 42}})
    assert G.link("A_B").costs["CAR"]["time"] == 42, "Modification through link() should be visible"
    link = G.find_link("B_C")
    assert link is not None
    link.update_costs({"BUS": {"time": 43}})
    assert G.link("B_C").costs["BUS"]["time"] == 43, "Modification through find_link() should be visible"

    # Accessors must stay consistent after a deletion.
    G.delete_link("A_B")
    assert G.link_count == 1, "link_count should be 1 after deletion"
    assert "A_B" not in G.links, "`in G.links` should be false after deletion"
    assert G.find_link("A_B") is None, "find_link() should return None after deletion"
    with pytest.raises(ValueError):
        G.link("A_B")
    assert list(G.links) == ["B_C"], "View should not contain deleted link"


def test_node_ranges() -> None:

    G = _make_graph()

    expected = ["A", "B", "C", "D"]
    assert sorted(G.nodes) == expected, "View should enumerate each node once"
    assert sorted(G.nodes.keys()) == expected, "Key view should enumerate each node once"
    assert sorted(node.id for node in G.nodes.values()) == expected, "Value view should enumerate each node once"
    assert sorted(node_id for node_id, _ in G.nodes.items()) == expected, "Item view should enumerate each node once"

    for node in G.nodes.values():
        assert node is G.node(node.id), "View should enumerate the nodes of the graph"
    for node_id, node in G.nodes.items():
        assert node.id == node_id, "Item view should associate each node with its ID"

    # Python-specific: dict-like features.
    assert len(G.nodes.keys()) == len(G.nodes.values()) == len(G.nodes.items()) == 4, "Views should have 4 elements"
    assert "A" in G.nodes.keys() and "Z" not in G.nodes.keys(), "`in G.nodes.keys()` should work"  # noqa: SIM118
    assert G.nodes["A"] is G.node("A"), "G.nodes[...] should return the node"
    with pytest.raises(KeyError):
        G.nodes["Z"]


def test_link_ranges() -> None:

    G = _make_graph()

    expected = ["A_B", "B_C"]
    assert sorted(G.links) == expected, "View should enumerate each link once"
    assert sorted(G.links.keys()) == expected, "Key view should enumerate each link once"
    assert sorted(link.id for link in G.links.values()) == expected, "Value view should enumerate each link once"
    assert sorted(link_id for link_id, _ in G.links.items()) == expected, "Item view should enumerate each link once"

    for link in G.links.values():
        assert link is G.link(link.id), "View should enumerate the links of the graph"
    for link_id, link in G.links.items():
        assert link.id == link_id, "Item view should associate each link with its ID"

    # Modification through the view.
    for link in G.links.values():
        link.update_costs({"CAR": {"time": 100}})
    for link in G.links.values():
        assert link.costs["CAR"]["time"] == 100, "Modification through the view should be visible"

    # Python-specific: dict-like features.
    assert len(G.links.keys()) == len(G.links.values()) == len(G.links.items()) == 2, "Views should have 2 elements"
    assert "A_B" in G.links.keys() and "Z" not in G.links.keys(), "`in G.links.keys()` should work"  # noqa: SIM118
    assert G.links["A_B"] is G.link("A_B"), "G.links[...] should return the link"
    with pytest.raises(KeyError):
        G.links["Z"]


def test_add_node() -> None:

    G = _make_graph()
    a = G.node("A")

    G.add_node("E", 4, 4, "NewNode")
    e = G.node("E")
    assert G.node_count == 5, "node_count should be 5 after insertion"
    assert e.id == "E" and e.position == [4, 4] and e.label == "NewNode", "node() should return the inserted node"

    # Duplicated ID.
    with pytest.raises(ValueError):
        G.add_node("A", 5, 5, "Duplicate")
    assert G.node_count == 5, "node_count should be unchanged after a failed insertion"
    assert G.node("A") is a, "Existing node should not be replaced by a failed insertion"
    assert a.position == [0, 0] and a.label == "", "Existing node should not be modified by a failed insertion"
    assert list(a.adj) == ["B"] and a.adj["B"] is G.link("A_B"), (
        "Links of the existing node should not be modified by a failed insertion"
    )


def test_add_link() -> None:

    G = _make_graph()
    ab = G.link("A_B")

    G.add_link("C_D", "C", "D", 30, {"CAR": {"time": 3}}, "CarLayer")
    cd = G.link("C_D")
    assert G.link_count == 3, "link_count should be 3 after insertion"
    assert cd.id == "C_D" and cd.upstream == "C" and cd.downstream == "D" and cd.length == 30, (
        "link() should return the inserted link"
    )
    assert G.node("C").adj["D"] is cd and G.node("D").radj["C"] is cd, (
        "Inserted link should be registered in the adjacency of its nodes"
    )

    # Duplicated ID.
    with pytest.raises(ValueError):
        G.add_link("A_B", "D", "A", 40, {"CAR": {"time": 4}}, "Duplicate")
    assert G.link_count == 3, "link_count should be unchanged after a failed insertion"
    assert G.link("A_B") is ab, "Existing link should not be replaced by a failed insertion"
    assert ab.upstream == "A" and ab.downstream == "B" and ab.length == 10 and ab.label == "CarLayer", (
        "Existing link should not be modified by a failed insertion"
    )
    assert len(G.node("D").adj) == 0 and len(G.node("A").radj) == 0, (
        "Failed insertion should not modify the adjacency of the nodes"
    )

    # Unknown upstream/downstream node.
    with pytest.raises(ValueError):
        G.add_link("Z_A", "Z", "A", 50, {"CAR": {"time": 5}}, "CarLayer")
    with pytest.raises(ValueError):
        G.add_link("A_Z", "A", "Z", 50, {"CAR": {"time": 5}}, "CarLayer")
    assert G.link_count == 3, "link_count should be unchanged after a failed insertion"
    assert "Z_A" not in G.links and "A_Z" not in G.links, "Failed insertion should not add any link"
    assert "Z" not in G.nodes, "Failed insertion should not add any node"
    assert len(G.node("A").adj) == 1 and len(G.node("A").radj) == 0, (
        "Failed insertion should not modify the adjacency of the nodes"
    )


def test_exits() -> None:

    G = OrientedGraph()
    G.add_node("A", 0, 0, "")

    exclude_movements = {"A": {"C"}}
    G.add_node("B", 2, 5, "", exclude_movements)

    G.add_node("C", 12.0, 43.0, "")

    G.add_node("D", 435, 345, "")
    G.add_link("A_B", "A", "B", 12, {"CAR": {"time": 12}}, "CarLayer")
    G.add_link("B_C", "B", "C", 12, {"CAR": {"time": 12}}, "CarLayer")
    G.add_link("B_D", "B", "D", 12, {"CAR": {"time": 12}}, "CarLayer")

    exits = G.node("B").get_exits("A")

    assert len(exits) == 1, "get_exits() should return one link"
    assert exits[0].downstream == "D", "Exit link should go to 'D'"
