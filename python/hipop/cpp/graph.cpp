#include <hipop/create.h>
#include <hipop/graph.h>

#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/vector.h>
#include <nanobind/operators.h>

#include <cstddef>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace nb = nanobind;
using namespace nb::literals;

namespace hipop_wrappers {


/**
 * Adaptator class, to make a OrientedGraph class look like a container of Node.
 */
struct NodeContainerView {

    using Element = hipop::Node;
    using Iterator = hipop::OrientedGraph::NodeIterator;

    struct KeySubview { hipop::OrientedGraph *graph; };
    struct ValueSubview { hipop::OrientedGraph *graph; };
    struct ItemSubview { hipop::OrientedGraph *graph; };

    hipop::OrientedGraph *graph; // never null

    [[nodiscard]] static std::size_t count(const hipop::OrientedGraph &graph) { return graph.nodeCount(); }
    [[nodiscard]] static hipop::Node *find(hipop::OrientedGraph &graph, const std::string &id) { return graph.findNode(id); }
    [[nodiscard]] static hipop::OrientedGraph::NodeRange range(hipop::OrientedGraph &graph) { return graph.nodes(); }
};


/**
 * Adaptator class, to make a OrientedGraph class look like a container of Link.
 */
struct LinkContainerView {

    using Element = hipop::Link;
    using Iterator = hipop::OrientedGraph::LinkIterator;

    struct KeySubview { hipop::OrientedGraph *graph; };
    struct ValueSubview { hipop::OrientedGraph *graph; };
    struct ItemSubview { hipop::OrientedGraph *graph; };

    hipop::OrientedGraph *graph; // never null

    [[nodiscard]] static std::size_t count(const hipop::OrientedGraph &graph) { return graph.linkCount(); }
    [[nodiscard]] static hipop::Link *find(hipop::OrientedGraph &graph, const std::string &id) { return graph.findLink(id); }
    [[nodiscard]] static hipop::OrientedGraph::LinkRange range(hipop::OrientedGraph &graph) { return graph.links(); }
};


/**
 * Python iterator over the given projection of the elements of the graph.
 */
template<typename View, typename Projection, nb::rv_policy::value RVPolicy = nb::rv_policy::automatic_reference_v>
static auto make_projecting_iterator(hipop::OrientedGraph &graph, const char *name) {

    class ProjectingIterator {

        typename View::Iterator it_;

    public:

        explicit ProjectingIterator(typename View::Iterator it) : it_(it) {}

        [[nodiscard]] decltype(auto) operator*() const { return Projection{}(*it_); }

        ProjectingIterator &operator++() {
            ++it_;
            return *this;
        }

        [[nodiscard]] bool operator==(const ProjectingIterator &other) const { return it_ == other.it_; }
        [[nodiscard]] bool operator!=(const ProjectingIterator &other) const { return it_ != other.it_; }
    };

    auto range = View::range(graph);
    return nb::make_iterator<RVPolicy>(
        nb::type<View>(), name, ProjectingIterator(range.begin()), ProjectingIterator(range.end()));
}


/**
 * Register the Python class `name` corresponding to `View` (`NodeContainerView` or `LinkContainerView`).
 */
template<typename View>
static void bind_graph_element_container_view(nb::module_ &m, const char *name) {

    using Element = typename View::Element;
    using KeySubview = typename View::KeySubview;
    using ValueSubview = typename View::ValueSubview;
    using ItemSubview = typename View::ItemSubview;

    // Auxiliary classes for the .keys() / .items() iterators
    // Remark: no projection is needed for the .values() iterator, as its value-type corresponds
    // to the value-type of the underlying C++ range/iterator objects.
    struct KeyProjection {
        const std::string &operator()(Element *element) const {
            return element->mid;
        }
    };
    struct ItemProjection {
        std::pair<const std::string &, Element *> operator()(Element *element) const {
            return { element->mid, element };
        }
    };

    nb::class_<View> viewClass(m, name);

    nb::class_<KeySubview>(viewClass, "KeySubview")
        .def("__len__", [](const KeySubview &v) { return View::count(*v.graph); })
        .def("__contains__", [](const KeySubview &v, const std::string &id) {
            return View::find(*v.graph, id) != nullptr;
        })
        .def("__iter__", [](const KeySubview &v) {
            return make_projecting_iterator<View, KeyProjection>(*v.graph, "KeyIterator");
        }, nb::keep_alive<0, 1>());

    nb::class_<ValueSubview>(viewClass, "ValueSubview")
        .def("__len__", [](const ValueSubview &v) { return View::count(*v.graph); })
        .def("__iter__", [](const ValueSubview &v) {
            auto range = View::range(*v.graph);
            return nb::make_iterator<nb::rv_policy::reference>(nb::type<View>(), "ValueIterator", range.begin(), range.end());
        }, nb::keep_alive<0, 1>());

    nb::class_<ItemSubview>(viewClass, "ItemSubview")
        .def("__len__", [](const ItemSubview &v) { return View::count(*v.graph); })
        .def("__iter__", [](const ItemSubview &v) {
            return make_projecting_iterator<View, ItemProjection, nb::rv_policy::reference>(*v.graph, "ItemIterator");
        }, nb::keep_alive<0, 1>());

    viewClass
        .def("__len__", [](const View &v) { return View::count(*v.graph); })
        .def("__contains__", [](const View &v, const std::string &id) {
            return View::find(*v.graph, id) != nullptr;
        })
        .def("__getitem__", [](const View &v, const std::string &id) {
            Element *element = View::find(*v.graph, id);
            if (element == nullptr) {
                throw nb::key_error();
            }
            return element;
        }, "id"_a, nb::rv_policy::reference)
        .def("__iter__", [](const View &v) {
            return make_projecting_iterator<View, KeyProjection>(*v.graph, "KeyIterator");
        }, nb::keep_alive<0, 1>())
        .def("keys", [](const View &v) { return KeySubview{ v.graph }; }, nb::keep_alive<0, 1>())
        .def("values", [](const View &v) { return ValueSubview{ v.graph }; }, nb::keep_alive<0, 1>())
        .def("items", [](const View &v) { return ItemSubview{ v.graph }; }, nb::keep_alive<0, 1>());
}


void graph(nb::module_ &m) {

    nb::class_<hipop::Link>(m, "Link")
        .def_ro("id", &hipop::Link::mid)
        .def_prop_ro("upstream", [](const hipop::Link &link) { return link.mup->mid; })
        .def_prop_ro("downstream", [](const hipop::Link &link) { return link.mdown->mid; })
        .def_ro("costs", &hipop::Link::mcosts)
        .def_ro("label", &hipop::Link::mlabel)
        .def_ro("length", &hipop::Link::mlength)
        .def("update_costs", &hipop::Link::updateCosts, "costs"_a);

    nb::class_<hipop::Node>(m, "Node")
        .def_ro("id", &hipop::Node::mid)
        .def_ro("position", &hipop::Node::mposition)
        .def_ro("adj", &hipop::Node::madj)
        .def_ro("radj", &hipop::Node::mradj)
        .def_ro("label", &hipop::Node::mlabel)
        .def_ro("exclude_movements", &hipop::Node::mexclude_movements)
        .def("get_exits", &hipop::Node::getExits, "predecessor"_a, nb::rv_policy::reference);

    bind_graph_element_container_view<NodeContainerView>(m, "NodeMap");
    bind_graph_element_container_view<LinkContainerView>(m, "LinkMap");

    nb::class_<hipop::OrientedGraph>(m, "OrientedGraph")
        .def(nb::init<>())

        // Node accessors
        .def_prop_ro("node_count", &hipop::OrientedGraph::nodeCount)
        .def("node", [](hipop::OrientedGraph &graph, const std::string &nodeId) {
            return graph.node(nodeId);
        }, "node_id"_a, nb::rv_policy::reference)
        .def("find_node", [](hipop::OrientedGraph &graph, const std::string &nodeId) {
            return graph.findNode(nodeId);
        }, "node_id"_a, nb::rv_policy::reference, nb::sig("def find_node(self, node_id: str) -> Node | None"))
        .def_prop_ro("nodes", [](hipop::OrientedGraph &graph) {
            return NodeContainerView{ &graph };
        }, nb::keep_alive<0, 1>())

        // Link accessors
        .def_prop_ro("link_count", &hipop::OrientedGraph::linkCount)
        .def("link", [](hipop::OrientedGraph &graph, const std::string &linkId) {
            return graph.link(linkId);
        }, "link_id"_a, nb::rv_policy::reference)
        .def("find_link", [](hipop::OrientedGraph &graph, const std::string &linkId) {
            return graph.findLink(linkId);
        }, "link_id"_a, nb::rv_policy::reference, nb::sig("def find_link(self, link_id: str) -> Link | None"))
        .def_prop_ro("links", [](hipop::OrientedGraph &graph) {
            return LinkContainerView{ &graph };
        }, nb::keep_alive<0, 1>())

        // Other operations
        .def("add_all_nodes_and_links", &hipop::OrientedGraph::AddAllNodesAndLinks, "graph"_a)
        .def("add_node", &hipop::OrientedGraph::AddNode,
            "node_id"_a, "x"_a, "y"_a, "label"_a, "exclude_movements"_a = mapsets())
        .def("add_link", &hipop::OrientedGraph::AddLink,
            "link_id"_a, "up"_a, "down"_a, "length"_a, "costs"_a, "label"_a = "_def")
        .def("delete_link", &hipop::OrientedGraph::DeleteLink, "link_id"_a)
        .def("delete_all_links_to_node", &hipop::OrientedGraph::DeleteAllLinksToNode, "node_id"_a)
        .def("update_link_costs", &hipop::OrientedGraph::UpdateLinkCosts, "link_id"_a, "costs"_a)
        .def("update_costs", &hipop::OrientedGraph::UpdateCosts, "link_id_to_costs"_a)
        .def("get_length", &hipop::OrientedGraph::getLength, "up"_a, "down"_a)
        .def("get_links_without_cost", &hipop::OrientedGraph::GetLinksWithoutCost,
            "cost_metric"_a, "label_to_cost_family"_a);

    nb::class_<hipop::OrientedGraphPath>(m, "OrientedGraphPath",
        "A path within an OrientedGraph, i.e. a sequence of consecutive links from an origin node "
        "to a destination node.\n\n"
        "WARNING: a path holds references to the nodes and links of its graph. The graph must be kept alive "
        "as long as the path is, and the nodes and links visited by the path must not be removed from it. "
        "Otherwise, using the path results in undefined behavior (likely a crash).")
        .def(nb::init<const hipop::Node *>(), "origin"_a)
        .def(nb::init<const hipop::Node *, std::vector<const hipop::Link *>>(), "origin"_a, "links"_a)
        .def(nb::self == nb::self,
            nb::sig("def __eq__(self, arg: object, /) -> bool")) // Overridden signature, to accept any obj type.
        // Remark: operator != is not bound, as Python deduces it from operator ==.
        .def(nb::hash(nb::self))
        .def("__len__", &hipop::OrientedGraphPath::size)
        .def("__bool__", [](const hipop::OrientedGraphPath &) {
            // An empty path must not be treated as an empty container, even if `__len__()` returns 0.
            return true;
        })
        .def("copy", [](const hipop::OrientedGraphPath &path) {
            return hipop::OrientedGraphPath(path);
        })
        .def("__copy__", [](const hipop::OrientedGraphPath &path) {
            return hipop::OrientedGraphPath(path);
        })
        .def("__deepcopy__", [](const hipop::OrientedGraphPath &path, const nb::typed<nb::dict, int, nb::any> &) {
            // Nodes and links belong to the graph, not to the path: they must not be copied, even for a deep copy.
            // Thus, same behavior for __copy__ and __deepcopy__ in case of OrientedGraphPath.
            return hipop::OrientedGraphPath(path);
        }, "memo"_a)
        .def("__repr__", [](const hipop::OrientedGraphPath &path) {
            std::ostringstream stream;
            stream << "OrientedGraphPath(" << path << ")";
            return stream.str();
        })
        .def_prop_ro("origin", &hipop::OrientedGraphPath::origin, nb::rv_policy::reference)
        .def_prop_ro("origin_id", &hipop::OrientedGraphPath::originId)
        .def_prop_ro("destination", &hipop::OrientedGraphPath::destination, nb::rv_policy::reference)
        .def_prop_ro("destination_id", &hipop::OrientedGraphPath::destinationId)
        .def("node", &hipop::OrientedGraphPath::node, "node_index"_a, nb::rv_policy::reference)
        .def("node_id", &hipop::OrientedGraphPath::nodeId, "node_index"_a)
        .def("link", &hipop::OrientedGraphPath::link, "link_index"_a, nb::rv_policy::reference)
        .def("link_id", &hipop::OrientedGraphPath::linkId, "link_index"_a)
        .def("nodes", [](const hipop::OrientedGraphPath &path) {
            auto range = path.nodes();
            return std::vector<const hipop::Node *>(range.begin(), range.end());
        }, nb::rv_policy::reference)
        .def("node_ids", &hipop::OrientedGraphPath::nodeIds)
        .def("links", &hipop::OrientedGraphPath::links, nb::rv_policy::reference)
        .def("link_ids", &hipop::OrientedGraphPath::linkIds)
        .def("has_loop", &hipop::OrientedGraphPath::hasLoop)
        .def("add", nb::overload_cast<const hipop::Link *>(&hipop::OrientedGraphPath::add), "link"_a)
        .def("add", nb::overload_cast<const hipop::OrientedGraphPath &>(&hipop::OrientedGraphPath::add), "other"_a)
        .def("sub_path", &hipop::OrientedGraphPath::subPath,
            "pos"_a, "link_count"_a = std::numeric_limits<std::size_t>::max())
        .def("starts_with", &hipop::OrientedGraphPath::startsWith, "p"_a)
        .def("ends_with", &hipop::OrientedGraphPath::endsWith, "p"_a);

    m.def("generate_manhattan", &hipop::makeManhattan, "n"_a, "link_length"_a);

    m.def("make_empty_path", &hipop::makeEmptyPath, "graph"_a, "origin_node_id"_a);

    m.def("make_path", &hipop::makePath, "graph"_a, "link_ids"_a);

    m.def("merge_oriented_graph", &hipop::mergeOrientedGraph, "graphs"_a);

    m.def("copy_graph", [](const hipop::OrientedGraph &graph) {
        return hipop::OrientedGraph(graph);
    }, "graph"_a);
}

}
