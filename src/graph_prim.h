#pragma once

#include "graph.h"

#include <queue>  // for std::priority_queue
#include <vector> // for std::vector

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
    void VisitMinimumSpanningTree_Prim(
        Graph<TId, TLen>& undirectedGraph,
        std::function<void(Edge<Vertex<TId, TLen>, TLen>& /*edge*/)> visit)
    {
		// Algorithm:
		// Build MST by continuesly adding edges to it, each time picking a new edge
		// that connects a discovered vertex (in MST) and an undiscovered vertex (not in MST).

		using vertex_type = Vertex<TId, TLen>;
		using edge_type = Edge<vertex_type, TLen>;

		// Maintain a min-heap for edges (key = edge length)
		std::priority_queue<edge_type*, std::vector<edge_type*>, EdgeLengthGreater<edge_type> > edge_min_heap;

		// Pick an arbitrary vertex from the graph and add it to MST (discovered = true)
		vertex_type* firstVertex = undirectedGraph.FindVertex([](const vertex_type&) -> bool { return true; });
		firstVertex->Discovered() = true;
		// Add edges of the first vertes to the min-heap
		VisitEdges<TId, TLen>(
			*firstVertex,
			[&edge_min_heap](vertex_type& /*neighbor*/, edge_type& edge)
			{
				edge_min_heap.push(&edge);
			});

		while (!edge_min_heap.empty())
		{
			// Take a minimal length edge from the min-heap
			edge_type* min_cost_edge = edge_min_heap.top();
			edge_min_heap.pop();

			// Get the undiscovered vertex if it exists, otherwise continue the loop
			vertex_type* node = nullptr;
			if (!min_cost_edge->From().Discovered())
				node = &min_cost_edge->From();
			else if (!min_cost_edge->To().Discovered())
				node = &min_cost_edge->To();
			else
				continue;

			// Add the undiscovered vertex to MST (discovered = true)
			node->Discovered() = true;

			// Add the undiscovered edges of the newly added vertex to the min-heap
			VisitEdges<TId, TLen>(
				*node,
				[&edge_min_heap](vertex_type& /*neighbor*/, edge_type& edge)
				{
					// insert an edge from discovered subgraph to undiscovered subgraph
					if (!edge.From().Discovered() || !edge.To().Discovered())
						edge_min_heap.push(&edge);
				});

			visit(*min_cost_edge);
		}

		// Clean up
		cs::ClearDiscovered(undirectedGraph);
	}
} // namespace cs
