#pragma once

#include "hipop/graph_path.h"

#include <utility>


namespace hipop {


    /**
     * Tuple (graph-path, cost-value).
     *
     * Convenient alias, used by most path search functions as their return type.
     */
    using PathCost = std::pair<OrientedGraphPath, double>;


}
