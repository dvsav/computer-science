#pragma once

#include "graph.h"

#include <functional> // for std:::function
#include <queue>      // for std:::queue

namespace cs
{
    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch_Directed(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = typename vertex_type::edge_type;

        vertex_type& root = graph.GetVertexById(root_id);

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

        ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch_Undirected(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = typename vertex_type::edge_type;

        vertex_type& root = graph.GetVertexById(root_id);

        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        // explored = true
        root.AuxData().boolValue = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitNeighbors<TId, TData, TLen>(
                /*root*/ v,
                [&wavefront](vertex_type& neighbor) -> bool
                {
                    if (!neighbor.AuxData().boolValue)
                    {
                        wavefront.push(&neighbor);
                        // explored = true
                        neighbor.AuxData().boolValue = true;
                    }
                    return false; // 'false' means 'continue visiting'
                }
            );
        }

        ClearAuxData(graph);
    }
}
