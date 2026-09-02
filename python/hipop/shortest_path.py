import functools
import time

from hipop.cpp.shortest_path import (
    dijkstra,
    dijkstra_single_source,
    floyd_warshall,
    parallel_dijkstra,
    parallel_dijkstra_single_source,
    parallel_dijkstra_heterogeneous_costs,
    parallel_k_intermodal_shortest_path,
    k_shortest_path,
    parallel_k_shortest_path,
    astar_euclidian_dist,
    compute_path_length,
    compute_path_cost,
    compute_paths_costs,
)

_EXPORTED_FUNCTIONS = (
    "dijkstra",
    "dijkstra_single_source",
    "floyd_warshall",
    "parallel_dijkstra",
    "parallel_dijkstra_single_source",
    "parallel_dijkstra_heterogeneous_costs",
    "parallel_k_intermodal_shortest_path",
    "k_shortest_path",
    "parallel_k_shortest_path",
    "astar_euclidian_dist",
    "compute_path_length",
    "compute_path_cost",
    "compute_paths_costs",
)

_stats = {name: {"calls": 0, "total_time": 0.0} for name in _EXPORTED_FUNCTIONS}


def _track_calls(func):

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        stats = _stats[func.__name__]
        start = time.perf_counter()
        try:
            return func(*args, **kwargs)
        finally:
            stats["calls"] += 1
            stats["total_time"] += time.perf_counter() - start

    return wrapper


for _name in _EXPORTED_FUNCTIONS:
    globals()[_name] = _track_calls(globals()[_name])
del _name


def report_hipop_call_stats():
    """Log the number of invocations and cumulative C++ execution time for each
    exported function that has been called at least once."""
    at_least_one_call = False
    for name, stats in _stats.items():
        if stats["calls"] > 0:
            at_least_one_call = True
            print(f"HiPOP fun {name:<40}:    #calls = {stats['calls']:>7},    run time = {stats['total_time']:>12.6f}")
    if not at_least_one_call:
        print("No HiPOP functions were called.")
