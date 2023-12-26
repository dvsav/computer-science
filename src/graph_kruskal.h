#pragma once

#include "graph.h"

#include <queue>  // for std::priority_queue
#include <vector> // for std::vector

namespace cs
{
    template<typename TId, typename TLen>
    void VisitMinimumSpanningTree_Kruskal(
        Graph<TId, TLen>& undirectedGraph,
        std::function<void(Edge<Vertex<TId, TLen>, TLen>& /*edge*/)> visit)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        // Init union-find (disjoint set) structure for each vertex
        undirectedGraph.VisitVertices(
            [](vertex_type& v)
            {
                v.template AuxData<vertex_type>() = &v;
            });

        // Maintain a min-heap for edges (key = edge length)
        std::priority_queue<edge_type*, std::vector<edge_type*>, EdgeLengthGreater<edge_type> > edge_min_heap;
        undirectedGraph.VisitEdges(
            [&edge_min_heap](edge_type& e)
            {
                edge_min_heap.push(&e);
            });

        size_t n_edges = 0;
        while (n_edges < undirectedGraph.VerticesNumber() - 1)
        {
            // Take a minimal length edge from the min-heap
            edge_type* min_cost_edge = edge_min_heap.top();
            edge_min_heap.pop();

            auto leader1 = getLeader(min_cost_edge->From());
            auto leader2 = getLeader(min_cost_edge->To());
            if (leader1 != leader2)
            {
                // min_cost_edge doesn't create a loop in MST
                merge(leader1, leader2);
                visit(*min_cost_edge);
                ++n_edges;
            }
        }

        // Clean up
        undirectedGraph.VisitVertices(
            [](vertex_type& v)
            {
                v.template AuxData<vertex_type>() = nullptr;
            });
    }
} // namespace cs
