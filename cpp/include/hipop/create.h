#pragma once

#include <hipop/graph.h>

namespace hipop
{

    /**
     * Construct a squared Manhattan graph.
     *
     * @param n Number of nodes on each side.
     */
    OrientedGraph makeManhattan(int n, double linkLength);

} // namespace hipop
