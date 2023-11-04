#pragma once

#include "graph.h"

#include <functional> // for std::function
#include <queue>      // for std::queue
#include <stack>      // for std::stack

namespace cs
{
    template<typename TId, typename TData, typename TLen>
    void BreadthFirstSearch_Directed(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // queue is what makes the search BREADTH-first search
        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        // explored = true
        root.AuxData().boolValue = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitOutNeighbors<TId, TData, TLen>(
                /*vertex*/ v,
                [&wavefront](vertex_type& neighbor) -> void
                {
                    if (!neighbor.AuxData().boolValue)
                    {
                        wavefront.push(&neighbor);
                        // explored = true
                        neighbor.AuxData().boolValue = true;
                    }
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

        vertex_type& root = graph.GetVertexById(root_id);

        // queue is what makes the search BREADTH-first search
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
                /*vertex*/ v,
                [&wavefront](vertex_type& neighbor) -> void
                {
                    if (!neighbor.AuxData().boolValue)
                    {
                        wavefront.push(&neighbor);
                        // explored = true
                        neighbor.AuxData().boolValue = true;
                    }
                }
            );
        }

        ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void DepthFirstSearch_Directed(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        // explored = true
        root.AuxData().boolValue = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();

            visit(v);

            track.pop();

            VisitOutNeighbors<TId, TData, TLen>(
                /*vertex*/ v,
                [&track](vertex_type& neighbor) -> void
                {
                    if (!neighbor.AuxData().boolValue)
                    {
                        track.push(&neighbor);
                        // explored = true
                        neighbor.AuxData().boolValue = true;
                    }
                }
            );
        }

        ClearAuxData(graph);
    }

    template<typename TId, typename TData, typename TLen>
    void DepthFirstSearch_Undirected(
        Graph<TId, TData, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TData, TLen>&)> visit)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        // explored = true
        root.AuxData().boolValue = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();

            visit(v);

            track.pop();

            VisitNeighbors<TId, TData, TLen>(
                /*root*/ v,
                [&track](vertex_type& neighbor) -> void
                {
                    if (!neighbor.AuxData().boolValue)
                    {
                        track.push(&neighbor);
                        // explored = true
                        neighbor.AuxData().boolValue = true;
                    }
                }
            );
        }

        ClearAuxData(graph);
    }
}
