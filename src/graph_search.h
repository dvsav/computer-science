#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <queue>      // for std::queue
#include <stack>      // for std::stack

namespace cs
{
    /**
     * @brief Performs breadth-first search in directed @p graph starting from
     * vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex.
     *
     * @param graph
     * @param root_id - identifier of the vertex from which the search begins.
     * @param visit - functor accepting reference to a vertex.
     * @param clearDiscovered - if true resets 'Discovered' flag to false for all vertices in the graph.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void BreadthFirstSearch_Directed(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearDiscovered = true)
    {
        using vertex_type = Vertex<TId, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);
        if (root.Discovered())
            return;

        // queue is what makes the search BREADTH-first search
        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        root.Discovered() = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitOutNeighbors<TId, TLen>(
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

        if (clearDiscovered)
            ClearDiscovered(graph);
    }

    /**
     * @brief Performs breadth-first search in undirected @p graph starting from
     * vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex.
     *
     * @param graph
     * @param root_id - identifier of the vertex from which the search begins.
     * @param visit - functor accepting reference to a vertex.
     * @param clearDiscovered - if true resets 'Discovered' flag to false for all vertices in the graph.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void BreadthFirstSearch_Undirected(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearDiscovered = true)
    {
        using vertex_type = Vertex<TId, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);
        if (root.Discovered())
            return;

        // queue is what makes the search BREADTH-first search
        std::queue<vertex_type*> wavefront;
        wavefront.push(&root);

        root.Discovered() = true;

        while (!wavefront.empty())
        {
            vertex_type& v = *wavefront.front();

            visit(v);

            wavefront.pop();

            VisitNeighbors<TId, TLen>(
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

        if (clearDiscovered)
            ClearDiscovered(graph);
    }

    /**
     * @brief Performs depth-first search in directed @p graph starting from
     * vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex.
     *
     * @param graph
     * @param root_id - identifier of the vertex from which the search begins.
     * @param visit - functor accepting reference to a vertex.
     * @param clearDiscovered - if true resets 'Discovered' flag to false for all vertices in the graph.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void DepthFirstSearch_Directed(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearDiscovered = true)
    {
        using vertex_type = Vertex<TId, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);
        if (root.Discovered())
            return;

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);
        root.Discovered() = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();
            visit(v);
            track.pop();

            VisitOutNeighbors<TId, TLen>(
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

        if (clearDiscovered)
            ClearDiscovered(graph);
    }

    /**
     * @brief Performs depth-first search in undirected @p graph starting from
     * vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex.
     *
     * @param graph
     * @param root_id - identifier of the vertex from which the search begins.
     * @param visit - functor accepting reference to a vertex.
     * @param clearDiscovered - if true resets 'Discovered' flag to false for all vertices in the graph.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void DepthFirstSearch_Undirected(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearDiscovered = true)
    {
        using vertex_type = Vertex<TId, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);
        if (root.Discovered())
            return;

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        root.Discovered() = true;

        while (!track.empty())
        {
            vertex_type& v = *track.top();

            visit(v);

            track.pop();

            VisitNeighbors<TId, TLen>(
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

        if (clearDiscovered)
            ClearDiscovered(graph);
    }

    /**
     * @brief Performs depth-first search in inverse-topological order in a directed @p graph starting from
     * the vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex. 'Inverse-topological order' assumes that
     * every call of @p visitor visits a vertex that doesn't have any undiscovered out-neighbors,
     * beginning from the 'sink' vertex (the one that doesn't have any outgoing edges) all the way to the
     * root vertex. True (iverse-)topological order is achievable only for directed acyclic graphs,
     * but the algorithm is still valid for directed graphs having loops in them (e.g. in Kosaraju algorithm).
     *
     * @param graph
     * @param root_id - identifier of the vertex from which the search begins.
     * @param visit - functor accepting reference to a vertex.
     * @param clearDiscovered - if true resets 'Discovered' flag to false for all vertices in the graph.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void DepthFirstSearch_Directed_InverseTopological(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearDiscovered = true)
    {
        using vertex_type = Vertex<TId, TLen>;

        vertex_type& root = graph.GetVertexById(root_id);
        if (root.Discovered())
            return;

        // stack is what makes the search DEPTH-first search
        std::stack<vertex_type*> track;
        track.push(&root);

        while (!track.empty())
        {
            vertex_type& v = *track.top();
            if (v.Discovered())
            {
                // If we've already discovered this vertex, that means that
                // we've already discovered all its outgoing neighbors, so
                // there's no need to try to discover them again.
                // Why can't we pop the vertex BEFORE we discover its neighbors?
                // Because we have to visit the vertex AFTER we've discovered
                // its neighbors.
                visit(v);
                track.pop();
            }
            else
            {
                v.Discovered() = true;

                bool no_outgoing_edges = true;
                VisitOutNeighbors<TId, TLen>(
                    /*vertex*/ v,
                    [&no_outgoing_edges, &track](vertex_type& neighbor) -> void
                    {
                        if (!neighbor.Discovered())
                        {
                            track.push(&neighbor);
                            no_outgoing_edges = false;
                        }
                    }
                );

                if (no_outgoing_edges)
                {
                    // There's no undiscovered outgoing neighbors
                    visit(v);
                    track.pop();
                }
            }
        }

        if (clearDiscovered)
            ClearDiscovered(graph);
    }
} // namespace cs
