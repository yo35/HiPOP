#pragma once

#include "hipop/graph.h"
#include "hipop/graph_path.h"

#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>


namespace hipop {


    /**
     * Tuple (graph-path, cost-value).
     *
     * Convenient alias, used by most path search functions as their return type.
     */
    using PathCost = std::pair<OrientedGraphPath, double>;


    /**
     * Compute the cost of a path, i.e. the sum of the cost value assigned to each link composing the path.
     *
     * How a cost value is assigned to each link is defined by the given cost function.
     *
     * @param costFunction Must be a callable object with the following signature: `double(const Link *link)`.
     *                     Should be a pure function (i.e. no side effects).
     */
    template<typename CostFunction>
    double computePathCostGeneric(const OrientedGraphPath &path, const CostFunction &costFunction) {
        static_assert(
            std::is_invocable_r_v<double, CostFunction, const Link *>,
            "Cost function must have signature: double(const Link *link)"
        );
        double result = 0;
        for (const Link *link : path.links()) {
            result += costFunction(link);
        }
        return result;
    }


    /**
     * Total length of a path, i.e. sum of all link->length() that composed it.
     */
    inline double computePathLength(const OrientedGraphPath &path) {
        return computePathCostGeneric(path, [](const Link *link) { return link->mlength; });
    }


    /**
     * Total cost of a path, using the given criteria to pick a cost family / cost metric on each link.
     *
     * @param labelToCostFamily Cost family to use for each link label.
     * @param costMetric Cost metric to consider.
     */
    inline double computePathCost(
        const OrientedGraphPath &path,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const std::string &costMetric)
    {
        return computePathCostGeneric(path, [&labelToCostFamily, &costMetric](const Link *link) {
            return link->cost(labelToCostFamily.at(link->mlabel), costMetric);
        });
    }


}
