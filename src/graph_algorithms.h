#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <queue>      // for std::queue
#include <stack>      // for std::stack

namespace cs
{
    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch_Directed(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit,
        bool clearAuxData = true)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // queue is what makes the search BREADTH-first search
        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        root.Discovered() = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitOutNeighbors<TId, TData, TLen>(
                /*vertex*/ v,
                [&wavefront](vertex_type& neighbor) -> void
                {
                    if (!neighbor.Discovered())
                    {
                        wavefront.push(&neighbor);
                        neighbor.Discovered() = true;
                    }
                }
            );
        }

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch_Undirected(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit,
        bool clearAuxData = true)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // queue is what makes the search BREADTH-first search
        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        root.Discovered() = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitNeighbors<TId, TData, TLen>(
                /*vertex*/ v,
                [&wavefront](vertex_type& neighbor) -> void
                {
                    if (!neighbor.Discovered())
                    {
                        wavefront.push(&neighbor);
                        neighbor.Discovered() = true;
                    }
                }
            );
        }

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void DepthFirstSearch_Directed(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit,
        bool clearAuxData = true)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        root.Discovered() = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();

            visit(v);

            track.pop();

            VisitOutNeighbors<TId, TData, TLen>(
                /*vertex*/ v,
                [&track](vertex_type& neighbor) -> void
                {
                    if (!neighbor.Discovered())
                    {
                        track.push(&neighbor);
                        neighbor.Discovered() = true;
                    }
                }
            );
        }

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void DepthFirstSearch_Undirected(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit,
        bool clearAuxData = true)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        root.Discovered() = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();

            visit(v);

            track.pop();

            VisitNeighbors<TId, TData, TLen>(
                /*root*/ v,
                [&track](vertex_type& neighbor) -> void
                {
                    if (!neighbor.Discovered())
                    {
                        track.push(&neighbor);
                        neighbor.Discovered() = true;
                    }
                }
            );
        }

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void TopologicalSort(
        Graph<TId, TData, TLen>& graph,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        std::vector<vertex_type*> topological_order(graph.VerticesNumber(), nullptr);
        size_t index = topological_order.size() - 1;

        graph.VisitVertices(
            [&graph, &topological_order, &index](vertex_type& v)
            {
                if (!v.Discovered())
                {
                    std::stack<vertex_type*> dfs_stack;

                    DepthFirstSearch_Directed<TId, TData, TLen>(
                        /*graph*/ graph,
                        /*root_id*/ v.Id(),
                        /*visit*/ [&dfs_stack](vertex_type& u) { dfs_stack.push(&u); },
                        /*clearAuxData*/ false);

                    while (!dfs_stack.empty())
                    {
                        topological_order[index--] = dfs_stack.top();
                        dfs_stack.pop();
                    }
                }
            }
        );

        ClearAuxData(graph);

        for (vertex_type* v : topological_order)
            visit(*v);
    }

    template<typename TId, typename TData, typename TLen>
    void FindStronglyConnectedComponents_Kosaraju(
        Graph<TId, TData, TLen>& graph)
    {
        // TODO
    }
}
