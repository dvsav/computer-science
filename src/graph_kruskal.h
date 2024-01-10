#pragma once

#include "graph.h"

#include <cstddef> // for size_t
#include <queue>   // for std::priority_queue
#include <vector>  // for std::vector

namespace cs
{
    /**
     * @brief Visits the edges of the Minimum Spanning Tree (MST) of the graph.
     * MST is a subgraph connecting all vertices of the graph and having minimal
     * total cost (length) of its edges (and consequently no loops).
     * Complexity: O(MlogN) where N is the number of vertices of the graph,
     * M is the number of graph edges.
     * @param undirectedGraph - undirected connected graph.
     * @param visit - functor to call on each edge of the MST.
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitMinimumSpanningTree_Kruskal(
        Graph<TId, TLen>& undirectedGraph,
        std::function<void(Edge<Vertex<TId, TLen>, TLen>& /*edge*/)> visit)
    {
        // Algorithm:
        // Put all graph edges in a min-heap (key = edge length).
        // Iteratively pop an edge from the top of min-heap and if
        // this edge doesn't create a loop in MST, add it to MST.
        // Whether or not a particular edge creates a loop in MST
        // is determined via union-find structures (aka disjoint sets).

        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        // Init union-find (disjoint set) structure for each vertex
        // which in our case is just a pointer to a "leader" vertex.
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

        // Finds the leader (connected component) of a given vertex.
        // Connected component is identified by a pointer to vertex.
        auto getLeader = [](vertex_type& v) -> vertex_type*
        {
            // Find leader
            vertex_type* u = &v;
            do
            {
                u = u->template AuxData<vertex_type>();
            } while (u->template AuxData<vertex_type>() != u);

            vertex_type* leader = u;

            // Compress path
            u = &v;
            while (true)
            {
                vertex_type* temp = u->template AuxData<vertex_type>();
                if (temp == leader)
                    break;
                u->template AuxData<vertex_type>() = leader;
                u = temp;
            }

            return leader;
        };

        // Merges (union) two connected components.
        auto merge = [](vertex_type& v, vertex_type& u) -> void
        {
            vertex_type* v_leader = v.template AuxData<vertex_type>();
            vertex_type* u_leader = u.template AuxData<vertex_type>();

            u_leader->template AuxData<vertex_type>() = v_leader;
            u.template AuxData<vertex_type>() = v_leader;
        };

        size_t n_edges = 0;
        while (n_edges < undirectedGraph.VerticesNumber() - 1)
        {
            // Take a minimal length edge from the min-heap
            edge_type* min_cost_edge = edge_min_heap.top();
            edge_min_heap.pop();

            // Find the leaders (connected components) to which the vertices belong
            auto leader1 = getLeader(min_cost_edge->From());
            auto leader2 = getLeader(min_cost_edge->To());
            // If they belong to the different connected components => edge doesn't create a loop 
            if (leader1 != leader2)
            {
                // min_cost_edge doesn't create a loop in MST
                merge(min_cost_edge->From(), min_cost_edge->To());
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
