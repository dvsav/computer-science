#pragma once

#include "graph.h"

#include <queue>  // for std::priority_queue
#include <vector> // for std::vector

namespace cs
{
	/**
	 * @class EdgeLengthGreater
	 * Compares two edges by their length (cost).
	 * @tparam TEdge edge type.
	 */
	template<typename TEdge>
	class EdgeLengthGreater
	{
	public:
		bool operator() (
			const TEdge* lhs,
			const TEdge* rhs)
		{
			return lhs->Length() > rhs->Length();
		}
	};

	/**
	 * @brief Visits the edges of the Minimum Spanning Tree (MST) of the graph.
	 * MST is a subgraph connecting all vertices of the graph and having minimal
	 * total cost (length) of its edges.
	 * Complexity: O(NlogN) where N is the number of vertices of the graph.
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
        using vertex_type = Vertex<TId, TLen>;
		using edge_type = Edge<vertex_type, TLen>;

		std::priority_queue<edge_type*, std::vector<edge_type*>, EdgeLengthGreater<edge_type> > edge_min_heap;
		vertex_type* firstVertex = undirectedGraph.FindVertex([](const vertex_type&) -> bool { return true; });
		firstVertex->Discovered() = true;

		VisitEdges<TId, TLen>(
			*firstVertex,
			[&edge_min_heap](vertex_type& /*neighbor*/, edge_type& edge)
			{
				edge_min_heap.push(&edge);
			});

		while (!edge_min_heap.empty())
		{
			edge_type* min_cost_edge = edge_min_heap.top();
			edge_min_heap.pop();

			vertex_type* node = nullptr;
			if (!min_cost_edge->From().Discovered())
				node = &min_cost_edge->From();
			else if (!min_cost_edge->To().Discovered())
				node = &min_cost_edge->To();
			else
				continue;

			node->Discovered() = true;
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

		cs::ClearDiscovered(graph);
	}
} // namespace cs
