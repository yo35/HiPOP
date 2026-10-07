#pragma once

#include "hipop/graph.h"

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>


using pathCost = std::pair<std::vector<std::string>, double>;
using ShortestPathsTree = std::unordered_map<std::string, std::string>;

namespace hipop
{

    /**
     * Compute the cost of a path, i.e. the sum of the cost value assigned to each link composing the path.
     *
     * How a cost value is assigned to each link is defined by the given cost function.
     *
     * @param costFunction Must be a callable object with the following signature: `double(const Link *link)`.
     *                     Should be a pure function (i.e. no side effects).
     */
    template<typename CostFunction>
    double computePathCostGeneric(
        const OrientedGraph &G,
        const std::vector<std::string> &path,
        const CostFunction &costFunction)
    {
        static_assert(
            std::is_invocable_r_v<double, CostFunction, const Link *>,
            "Cost function must have signature: double(const Link *link)"
        );
        double result = 0;
        for (std::size_t i = 0; i + 1 < path.size(); ++i) {
            const Link *link = G.mnodes.at(path[i])->madj.at(path[i + 1]);
            result += costFunction(link);
        }
        return result;
    }


    /**
     * Total length of a path, i.e. sum of all link->mlength that composed it.
     */
    inline double computePathLength(const OrientedGraph &G, const std::vector<std::string> &path) {
        return computePathCostGeneric(G, path, [](const Link *link) { return link->mlength; });
    }


    /**
     * Total cost of a path, using the given criteria to pick a cost family / cost metric on each link.
     *
     * @param costMetric Cost metric to consider.
     * @param labelToCostFamily Cost family to use for each link label.
     */
    inline double computePathCost(const OrientedGraph &G,
        const std::vector<std::string> &path,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily)
    {
        return computePathCostGeneric(G, path, [&labelToCostFamily, &costMetric](const Link *link) {
            return link->cost(labelToCostFamily.at(link->mlabel), costMetric);
        });
    }


    std::vector<std::vector<double>> computePathsCosts(OrientedGraph &G,
        const std::vector<std::vector<std::vector<std::string>>> &paths,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        int threadNumber);


    /**
     * Compute the shortest path between origin and destination using the Dijkstra algorithm.
     *
     * How a cost value is assigned to each link is defined by the given cost function.
     *
     * @param costFunction Should be a pure function (i.e. no side effects).
     * @param accessibleLinkLabels If non-empty, only the links whose label is in this set
     *                             are considered for the path search.
     * @return The list of Nodes defining the shortest path and the associated cost.
     */
    pathCost dijkstraGeneric(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::function<double(const Link *)> &costFunction,
        const setstring &accessibleLinkLabels);


    /**
     * Compute the shortest path between origin and destination using the Dijkstra algorithm.
     *
     * @param costMetric Cost metric to consider.
     * @param labelToCostFamily Cost family to use for each link label.
     * @param accessibleLinkLabels If non-empty, only the links whose label is in this set
     *                             are considered for the path search.
     * @return The list of Nodes defining the shortest path and the associated cost.
     */
    inline pathCost dijkstra(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels = {})
    {
        return dijkstraGeneric(G, origin, destination, [&labelToCostFamily, &costMetric](const Link *link) {
            return link->cost(labelToCostFamily.at(link->mlabel), costMetric);
        }, accessibleLinkLabels);
    }


    ShortestPathsTree dijkstraSingleSource(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels);
    std::pair<std::vector<std::vector<int>>, std::unordered_map<int, std::string>> floydWarshall(
        const OrientedGraph &G,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels);
    pathCost aStar(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels,
        const std::function<double(const Node *, const Node *)> &heuristic);
    pathCost aStarEuclidianDist(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels);

    std::vector<pathCost> parallelDijkstra(
        const OrientedGraph &G,
        const std::vector<std::string> &origins,
        const std::vector<std::string> &destinations,
        const std::vector<std::unordered_map<std::string, std::string>> &labelToCostFamily,
        const std::string &costMetric,
        int threadNumber,
        const std::vector<setstring> &accessibleLinkLabels = {});

    std::vector<ShortestPathsTree> parallelDijkstraSingleSource(
        const OrientedGraph &G,
        const std::vector<std::string> &origins,
        const std::vector<std::unordered_map<std::string, std::string>> &labelToCostFamily,
        const std::string &costMetric,
        int threadNumber,
        const std::vector<setstring> &accessibleLinkLabels = {});

    std::vector<pathCost> parallelDijkstraHeterogeneousCosts(
        const OrientedGraph &G,
        const std::vector<std::string> &origins,
        const std::vector<std::string> &destinations,
        const std::vector<std::unordered_map<std::string, std::string>> &labelToCostFamily,
        const std::vector<std::string> &costMetrics,
        int threadNumber,
        const std::vector<setstring> &accessibleLinkLabels = {});

    std::vector<pathCost> YenKShortestPath(
        OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const setstring &accessibleLinkLabels,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        int kPath);
    std::vector<pathCost> KShortestPath(
        OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const setstring &accessibleLinkLabels,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        double maxDiffCost,
        double maxDistInCommon,
        double costMultiplier,
        int maxRetry,
        int kPath,
        bool intermodal);

    std::vector<std::vector<pathCost>> parallelKShortestPath(
        OrientedGraph &G,
        const std::vector<std::string> &origins,
        const std::vector<std::string> &destinations,
        const std::string &costMetric,
        const std::vector<std::unordered_map<std::string, std::string>> &labelToCostFamily,
        const std::vector<setstring> &accessibleLinkLabels,
        double maxDiffCost,
        double maxDistInCommon,
        double costMultiplier,
        int maxRetry,
        const std::vector<int> &kPaths,
        int threadNumber);

    std::vector<std::vector<pathCost>> parallelKIntermodalShortestPath(
        const OrientedGraph &G,
        const std::vector<std::string> &origins,
        const std::vector<std::string> &destinations,
        const std::vector<std::unordered_map<std::string, std::string>> &labelToCostFamily,
        const std::string &costMetric,
        int threadNumber,
        const std::pair<std::unordered_set<std::string>, std::unordered_set<std::string>> &pairMandatoryLabels,
        double maxDiffCost,
        double maxDistInCommon,
        double costMultiplier,
        int maxRetry,
        const std::vector<int> &kPaths,
        const std::vector<setstring> &accessibleLinkLabels = {});

} // namespace hipop
