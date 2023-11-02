#pragma once

#include "graph.h"

#include <functional> // for std:::function
#include <queue>      // for std:::queue

namespace cs
{
    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch(
        Vertex<TId, TData, TLen>& root,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = typename vertex_type::edge_type;

        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        // explored = true
        root.AuxData().boolValue = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            v.VisitOutgoingEdges(
                [&wavefront](edge_type& e) -> bool
                {
                    if (!e.To().AuxData().boolValue)
                    {
                        wavefront.push(&e.To());
                        // explored = true
                        e.To().AuxData().boolValue = true;
                    }
                    return false; // 'false' means 'continue visiting'
                }
            );
        }
    }
}
