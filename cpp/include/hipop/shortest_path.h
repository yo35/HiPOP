#pragma once

#include "hipop/graph.h"
#include "hipop/graph_path_cost.h"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// TODO remove type aliases pathCost and ShortestPathsTree
using pathCost = std::pair<std::vector<std::string>, double>;
using ShortestPathsTree = std::unordered_map<std::string, std::string>;

namespace hipop {

    std::optional<PathCost> dijkstra(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels = {});

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

    std::optional<PathCost> aStar(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels,
        const std::function<double(const Node *, const Node *)> &heuristic);

    std::optional<PathCost> aStarEuclidianDist(
        const OrientedGraph &G,
        const std::string &origin,
        const std::string &destination,
        const std::string &costMetric,
        const std::unordered_map<std::string, std::string> &labelToCostFamily,
        const setstring &accessibleLinkLabels);

    std::vector<std::optional<PathCost>> parallelDijkstra(
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

    std::vector<std::optional<PathCost>> parallelDijkstraHeterogeneousCosts(
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

    std::vector<PathCost> KShortestPath(
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

    std::vector<std::vector<PathCost>> parallelKShortestPath(
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

    std::vector<std::vector<PathCost>> parallelKIntermodalShortestPath(
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
