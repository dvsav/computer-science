#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <queue>      // for std::queue
#include <stack>      // for std::stack
#include <utility>    // for std::pair
#include <list>       // for std::list
#include <vector>     // for std::vector
#include <algorithm>  // for std::upper_bound
#include <limits>     // for std::numeric_limits

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

    template<typename TLen>
    TLen GetInfinity() 
    {
        if (std::numeric_limits<TLen>::has_infinity)
            return std::numeric_limits<TLen>::infinity();
        return std::numeric_limits<TLen>::max();
    }

    template<typename TId, typename TLen>
    std::pair<std::list<TId>, TLen> FindShortestPath_Dijkstra(
        Graph<TId, std::pair<TId, TLen>, TLen>& graph,
        TId start_id,
        TId finish_id,
        std::function<void(Vertex<TId, std::pair<TId, TLen>, TLen>&)> visit,
        bool clearAuxData = true)
    {
        using vertex_type = Vertex<TId, std::pair<TId, TLen>, TLen>;

        // initialize data:
        // path lengths from start vertex to other verteces is infinite 
        const TLen max_length = GetInfinity<TLen>(); 
        graph.VisitVertices([max_length](vertex_type& vertex) -> void { vertex.Data().second = max_length; });
        // path length from start vertex to itself is zero 
        vertex_type& start = graph.GetVertexById(start_id);
        start.Data().second = 0;

        // list is used because we need to insert vertices in order by path length from the start
        // list will be maintained in sorted state
        std::list<vertex_type*> vertices;
        vertices.push_back(&start);

        while (!vertices.empty() && vertices.front()->Id() != finish_id) 
        {
            vertex_type& v = *vertices.front();    
            visit(v);
            vertices.pop_front();
            v.Discovered() = true;
            TLen v_length = v.Data().second;

            VisitOutNeighbors<TId, std::pair<TId, TLen>, TLen>(
                /*vertex*/ v,
                /*visit*/
                [&vertices, &graph, v_length, &v](vertex_type& neighbor) -> void
                {
                    if (!neighbor.Discovered())
                    {
                        TLen edge_length = graph.GetEdgeByIds(v.Id(), neighbor.Id()).Length();
                        TLen total_length = v_length + edge_length;

                        if (total_length < neighbor.Data().second)
                        {
                            neighbor.Data().first = v.Id();
                            neighbor.Data().second = total_length;

                            auto pos = std::upper_bound(vertices.begin(), vertices.end(), total_length,
                                    [](TLen l, const vertex_type* v) { return l < v->Data().second; });
                            vertices.insert(pos, &neighbor);
                        }
                    }
                }
            );
        }

        // if no way found from start to finish return empty path and max (infinite) length
        vertex_type& finish = graph.GetVertexById(finish_id);
        if (finish.Data().second == max_length) 
            return {{}, max_length};

        // otherwise construct the path by backtracking from finish to start
        std::list<TId> path;
        for (TId id = finish_id; id != start_id; )
        {
            path.push_front(id);
            vertex_type& v = graph.GetVertexById(id);
            id = v.Data().first;
        }
        path.push_front(start_id);

        // total length is length of path from start to finish
        TLen total_length = finish.Data().second;

        if (clearAuxData)
            graph.VisitVertices([](vertex_type& vertex) -> void { vertex.Data() = {}; });
    
        // return the path from start to finish and path's length
        return {path, total_length};
    }
}
