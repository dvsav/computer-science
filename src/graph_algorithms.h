#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <iterator>   // for std::iterator_traits
#include <queue>      // for std::queue
#include <stack>      // for std::stack

namespace cs
{
    template<typename TId, typename TLen>
    void BreadthFirstSearch_Directed(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearAuxData = true)
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

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TLen>
    void BreadthFirstSearch_Undirected(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearAuxData = true)
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

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TLen>
    void DepthFirstSearch_Directed(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearAuxData = true)
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

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TLen>
    void DepthFirstSearch_Undirected(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearAuxData = true)
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

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TLen>
    void DepthFirstSearch_Directed_InverseTopological(
        Graph<TId, TLen>& graph,
        TId root_id,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool clearAuxData = true)
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
                visit(v);
                track.pop();
            }
        }

        if (clearAuxData)
            ClearAuxData(graph);
    }

    template<typename TId, typename TLen>
    void TopologicalSort(
        Graph<TId, TLen>& graph,
        std::function<void(Vertex<TId, TLen>&)> visit,
        bool visit_in_reverse_order = false)
    {
        using vertex_type = Vertex<TId, TLen>;

        std::vector<vertex_type*> topological_order(graph.VerticesNumber(), nullptr);
        size_t index = topological_order.size() - 1;

        graph.VisitVertices(
            [&graph, &topological_order, &index](vertex_type& v)
            {
                if (!v.Discovered())
                {
                    DepthFirstSearch_Directed_InverseTopological<TId, TLen>(
                        /*graph*/ graph,
                        /*root_id*/ v.Id(),
                        /*visit*/ [&topological_order, &index](vertex_type& u) { topological_order[index--] = &u; },
                        /*clearAuxData*/ false);
                }
            }
        );

        ClearAuxData(graph);

        if (visit_in_reverse_order)
        {
            for (auto i = topological_order.rbegin(); i != topological_order.rend(); i++)
            {
                vertex_type* v = *i;
                visit(*v);
            }
        }
        else
        {
            for (auto i = topological_order.begin(); i != topological_order.end(); i++)
            {
                vertex_type* v = *i;
                visit(*v);
            }
        }
    }

    template<typename TId, typename TLen, typename TIterator>
    bool IsTopologicalOrder(TIterator begin, TIterator end)
    {
        // Check that TIterator contains pointer to Vertex<TId, TLen>
        static_assert(
            std::is_same<typename std::iterator_traits<TIterator>::value_type, typename cs::Vertex<TId, TLen>* >::value,
            "TIterator has the wrong value type");

        // Check that TIterator is a random-access iterator
        static_assert(
            std::is_same<typename std::iterator_traits<TIterator>::iterator_category,
            std::random_access_iterator_tag>::value,
            "TIterator must be a random-access iterator.");

        for (TIterator i = begin + 1; i != end; i++)
        {
            for (TIterator j = begin; j != i; j++)
            {
                if (DirectedEdgeExists<TId, TLen>(/*from*/ **i, /*to*/ **j))
                    return false;
            }
        }
        return true;
    }

    template<typename TId, typename TLen>
    void VisitStronglyConnectedComponents_Kosaraju(
        Graph<TId, TLen>& graph,
        std::function<void(Vertex<TId, TLen>& /*vertex*/, TId /*scc_id*/)> visit)
    {
        TopologicalSort<TId, TLen>(
            /*graph*/ graph,
            /*visit*/
            [&graph, visit](Vertex<TId, TLen>& vertex)
            {
                TId scc_id = vertex.Id();
                DepthFirstSearch_Directed<TId, TLen>(
                    /*graph*/ graph,
                    /*root_id*/ vertex.Id(),
                    /*visit*/
                    [scc_id, visit](Vertex<TId, TLen>& successor_of_vertex) -> void
                    {
                        visit(successor_of_vertex, scc_id);
                    },
                    /*clearAuxData*/ false
                );
            },
            /*visit_in_reverse_order*/ true
        );
        ClearAuxData(graph);
    }
}
