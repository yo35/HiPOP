#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/vector.h>

#include <hipop/graph.h>
#include <hipop/create.h>

namespace nb = nanobind;

namespace hipop_wrappers {

void graph(nb::module_ &m) {
    nb::class_<hipop::Link>(m, "Link")
        .def_ro("id", &hipop::Link::mid)
        .def_prop_ro("upstream", [](const hipop::Link &link) { return link.mup->mid; })
        .def_prop_ro("downstream", [](const hipop::Link &link) { return link.mdown->mid; })
        .def_ro("costs", &hipop::Link::mcosts)
        .def_ro("label", &hipop::Link::mlabel)
        .def_ro("length", &hipop::Link::mlength)
        .def("update_costs", &hipop::Link::updateCosts);

    nb::class_<hipop::Node>(m, "Node")
          .def_ro("id", &hipop::Node::mid)
          .def_ro("position", &hipop::Node::mposition)
          .def_ro("adj", &hipop::Node::madj)
          .def_ro("radj", &hipop::Node::mradj)
          .def_ro("label", &hipop::Node::mlabel)
          .def_ro("exclude_movements", &hipop::Node::mexclude_movements)
          .def("get_exits", &hipop::Node::getExits, nb::arg("predecessor"), nb::rv_policy::reference);

    nb::class_<hipop::OrientedGraph>(m, "OrientedGraph")
          .def(nb::init<>())
          .def_rw("nodes", &hipop::OrientedGraph::mnodes)
          .def_rw("links", &hipop::OrientedGraph::mlinks)
          .def("add_node", &hipop::OrientedGraph::AddNode, nb::arg("id"), nb::arg("x"), nb::arg("y"), nb::arg("label"), nb::arg("exclude_movements") = mapsets())
          .def("add_link", &hipop::OrientedGraph::AddLink,
               nb::arg("id"), nb::arg("up"), nb::arg("down"), nb::arg("length"), nb::arg("costs"), nb::arg("label") = "_def")
          .def("delete_link",&hipop::OrientedGraph::DeleteLink)
          .def("delete_all_links_to_node",&hipop::OrientedGraph::DeleteAllLinksToNode)
          .def("get_link", &hipop::OrientedGraph::getLink)
          .def("update_link_costs", &hipop::OrientedGraph::UpdateLinkCosts)
          .def("update_costs", &hipop::OrientedGraph::UpdateCosts)
          .def("get_length", &hipop::OrientedGraph::getLength)
          .def("get_links_without_cost", &hipop::OrientedGraph::GetLinksWithoutCost);

    m.def("generate_manhattan", &hipop::makeManhattan);

    m.def("merge_oriented_graph", &hipop::mergeOrientedGraph);

    m.def("copy_graph", [](const hipop::OrientedGraph &graph) {
        return new hipop::OrientedGraph(graph);
    });
}

}
