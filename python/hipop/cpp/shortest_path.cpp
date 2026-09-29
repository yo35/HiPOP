#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

#include <hipop/shortest_path.h>

namespace nb = nanobind;

namespace hipop_wrappers {

void shortest_path(nb::module_ &m) {
    m.def(
        "dijkstra",
        &hipop::dijkstra,
        nb::arg("graph"),
        nb::arg("origin"),
        nb::arg("destination"),
        nb::arg("cost"),
        nb::arg("map_label_cost"),
        nb::arg("available_labels") = setstring());
    m.def(
        "dijkstra_single_source",
        &hipop::dijkstraSingleSource,
        nb::arg("graph"),
        nb::arg("origin"),
        nb::arg("cost"),
        nb::arg("map_label_cost"),
        nb::arg("available_labels") = setstring());
    m.def(
      "floyd_warshall",
      &hipop::floydWarshall,
      nb::arg("graph"),
      nb::arg("cost"),
      nb::arg("map_label_cost"),
      nb::arg("available_labels") = setstring()
    );
    m.def(
        "parallel_dijkstra",
        &hipop::parallelDijkstra,
        nb::arg("graph"),
        nb::arg("origins"),
        nb::arg("destinations"),
        nb::arg("map_label_costs"),
        nb::arg("cost"),
        nb::arg("thread_number"),
        nb::arg("available_labels") = std::vector<setstring>());
    m.def(
        "parallel_dijkstra_single_source",
        &hipop::parallelDijkstraSingleSource,
        nb::arg("graph"),
        nb::arg("origins"),
        nb::arg("map_label_costs"),
        nb::arg("cost"),
        nb::arg("thread_number"),
        nb::arg("available_labels") = std::vector<setstring>());
    m.def(
        "parallel_dijkstra_heterogeneous_costs",
        &hipop::parallelDijkstraHeterogeneousCosts,
        nb::arg("graph"),
        nb::arg("origins"),
        nb::arg("destinations"),
        nb::arg("map_label_costs"),
        nb::arg("costs"),
        nb::arg("thread_number"),
        nb::arg("available_labels") = std::vector<setstring>());
    m.def("k_shortest_path", &hipop::KShortestPath);
    m.def("parallel_k_shortest_path", &hipop::parallelKShortestPath);
    m.def("yen_k_shortest_path", &hipop::YenKShortestPath);
    m.def("astar_euclidian_dist", &hipop::aStarEuclidianDist);
    m.def("compute_path_length", &hipop::computePathLength);
    m.def("compute_path_cost", &hipop::computePathCost);
    m.def("compute_paths_costs", &hipop::computePathsCosts);
    m.def(
        "parallel_k_intermodal_shortest_path",
        &hipop::parallelKIntermodalShortestPath,
        nb::arg("graph"),
        nb::arg("origins"),
        nb::arg("destinations"),
        nb::arg("map_label_costs"),
        nb::arg("cost"),
        nb::arg("thread_number"),
        nb::arg("pair_mandatory_labels"),
        nb::arg("max_diff_cost"),
        nb::arg("max_dist_in_common"),
        nb::arg("cost_multiplier"),
        nb::arg("max_retry"),
        nb::arg("nb_paths"),
        nb::arg("available_labels") = std::vector<setstring>());
}

}
