#pragma once

#include "graph.h"

#include <list>
#include <queue>  // for std::priority_queue
#include <vector> // for std::vector

namespace cs
{
	template<typename TId, typename TLen>
	class EdgeLengthGreater
	{
	public:
		using edge_type = Edge<TId, TLen>;

		bool operator() (
			const edge_type* lhs,
			const edge_type* rhs)
		{
			return lhs.Length() > rhs.Length();
		}
	};

    template<typename TId, typename TLen>
    void VisitMinimumSpanningTree_Prim(
        Graph<TId, TLen>& undirectedGraph,
        std::function<void(Edge<TId, TLen>& /*edge*/)> visit)
    {
        using vertex_type = Vertex<TId, TLen>;
		using edge_type = Edge<TId, TLen>;

		std::priority_queue<Edge*, std::vector<Edge*>, EdgeLengthGreater> edge_min_heap;
		vertex_type* firstVertex = undirectedGraph.FindVertex([](const vertex_type&) -> bool { return true; });
		firstVertex->Discovered() = true;

		VisitEdges(
			firstVertex,
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
			VisitEdges(
				*node,
				[&edge_min_heap](vertex_type& /*neighbor*/, edge_type& edge)
				{
					if (!edge.From().Discovered() || !edge.To().Discovered())
						edge_min_heap.push(&edge);
				});

			visit(*min_cost_edge);
		}
	}
} // namespace cs
