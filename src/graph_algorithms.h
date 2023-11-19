#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <iterator>   // for std::iterator_traits
#include <queue>      // for std::queue, std::priority_queue
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
     * @brief Performs depth-first search in inverse-topological order in directed @p graph starting from
     * vertex with identifier @p root_id calling functor @p visitor for each visited vertex.
     * Sets 'Discovered' flag to true for each visited vertex. 'Inverse-topological order' assumes that
     * every time a vertex is visited that doesn't have any undiscovered out-neighbors,
     * beginning from the 'sink' vertex (the one that doesn't have any outgoing edges) all the way to the
     * root vertex.
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

        if (clearDiscovered)
            ClearDiscovered(graph);
    }

    /**
     * @brief Calls functor @p visit for the vertices of @p graph in
     * topologically sorted order (or in reverse topologically sorted order
     * if @p visit_in_reverse_order is true). 'Topologically sorted order'
     * (suppose the sorted vertices go from left to right) assumes
     * that for each vertex v all v's incoming edges come from the left of v and
     * all v's outgoing edges go to the right of v. Note that this is true for
     * directed acyclic graphs only; however this sort is also used in graphs
     * having loops in Kosaraju's algorithm.
     * 
     * @see VisitStronglyConnectedComponents_Kosaraju.
     *
     * @param graph
     * @param visit - functor accepting reference to a vertex.
     * @param visit_in_reverse_order - if true, visits the vertices in reverse topologically sorted order.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
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
                        /*clearDiscovered*/ false);
                }
            }
        );

        ClearDiscovered(graph);

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

    /**
     * @brief Checks that vertices in a collection specified by a pair of
     * iterators @p begin and @p end are sorted in topological order.
     * 
     * @see TopologicalSort
     *
     * @param begin
     * @param end
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     * @tparam TIterator - iterator dereferencing to a pointer to vertex.
     * 
     * @return true if vertices are in topological order, false otherwise.
     */
    template<typename TId, typename TLen, typename TIterator>
    bool IsTopologicalOrder(
        TIterator begin,
        TIterator end)
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
                if (FindDirectedEdge<TId, TLen>(/*from*/ **i, /*to*/ **j))
                    return false;
            }
        }
        return true;
    }

    /**
     * @brief Performs the Kosaraju's algorithm of finding strongly connected components (SCC).
     * Calls functor @p visit for each vertex passing it an identifier of SCC this vertex belongs to.
     * SCC is a subgraph such that for each vertex v in that SCC there exists a path to all other
     * vertices of that SCC.
     *
     * @param graph
     * @param visit - functor accepting a reference to vertex and the identifier of
     * the strongly connected component (SCC) this vertex belongs to.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
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
                    /*clearDiscovered*/ false
                );
            },
            /*visit_in_reverse_order*/ true
        );
        ClearDiscovered(graph);
    }

    template <typename TId, typename TLen>
    struct DijkstraShortestPath_Data
    {
        using vertex_type = Vertex<TId, TLen>;

        TLen shortestPathLength;
        vertex_type* previousVertex;

        DijkstraShortestPath_Data() :
            shortestPathLength(0),
            previousVertex(nullptr)
        {}

        DijkstraShortestPath_Data(
            TLen shortestPathLength,
            vertex_type* previousVertex) :
            shortestPathLength(shortestPathLength),
            previousVertex(previousVertex)
        {}
    };

    template <typename TId, typename TLen>
    class DijkstraShortestPath_VertexMinHeap
    {
        using vertex_type = Vertex<TId, TLen>;

    private:
        class myComparator
        {
        public:
            int operator() (
                const vertex_type* lhs,
                const vertex_type* rhs)
            {
                return
                    lhs->template AuxData<DijkstraShortestPath_Data<TId, TLen> >()->shortestPathLength >
                    rhs->template AuxData<DijkstraShortestPath_Data<TId, TLen> >()->shortestPathLength;
            }
        };

    private:
        std::priority_queue<vertex_type*, std::vector<vertex_type*>, myComparator> vertices;

    public:
        void insert(vertex_type* v) { vertices.push(v); }

        bool empty() const { return vertices.empty(); }

        vertex_type* popClosestVertex()
        {
            auto v = vertices.top();
            vertices.pop();
            return v;
        }
    };

    /**
     * @brief Finds the minimum-length path in directed @p graph
     * from a vertex with identifier @p from_id to every other vertex in the @p graph.
     * For each vertex in the @p graph creates a structure DijkstraShortestPath_Data (containng
     * the shortest length to this vertex and the pointer to its predecessor in the shortest path)
     * and puts a pointer to that structure to the AuxData() attribute of the vertex - this structure
     * can be accessed afterwards if you've passed false as @p clearDijkstraData, but in that case
     * you are responsible for deallocating memory pointed to by AuxData() in each vertex.
     * Calls functor @p visitShortestPath (unless it is null) for each vertex in the path
     * from the vertex with identifier @p from_id to a vertex with identifier @p to_id.
     *
     * @see DijkstraShortestPath_Data
     * 
     * @param graph
     * @param from_id - identifier of a vertex which is the beginning of all shortest paths to be found.
     * @param to_id - identifier of a vertex which is the destination of the shortest path visited with @p visitShortestPath functor.
     * @param visitShortestPath - functor accepting a reference to vertex an the length of the shortest path from the
     * vertex with identifier @p @from_id.
     * @param clearDijkstraData - if true, clears all DijkstraShortestPath_Data structures created during the algorithm.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void DijkstraShortestPath_Directed(
        Graph<TId, TLen>& graph,
        TId from_id,
        TId to_id,
        std::function<void(Vertex<TId, TLen>& /*vertex*/, TLen /*shortestPathLength*/)> visitShortestPath,
        bool clearDijkstraData = true)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = typename vertex_type::edge_type;
        using dijkstra_type = DijkstraShortestPath_Data<TId, TLen>;

        vertex_type& from = graph.GetVertexById(from_id);
        vertex_type& to = graph.GetVertexById(to_id);

        from.template AuxData<dijkstra_type>() = new dijkstra_type;

        DijkstraShortestPath_VertexMinHeap<TId, TLen> frontier;
        frontier.insert(&from);

        // Score each vertex in the graph
        while (!frontier.empty())
        {
            vertex_type* current_node = frontier.popClosestVertex();
            if (current_node->Discovered()) continue;
            current_node->Discovered() = true;

            current_node->VisitOutgoingEdges(
                /*visitor*/
                [current_node, &frontier](edge_type& edge)
                {
                    auto& frontier_node = edge.To();

                    if (frontier_node.Discovered())
                        return;

                    TLen newLen = current_node->template AuxData<dijkstra_type>()->shortestPathLength + edge.Length();

                    if (frontier_node.template AuxData<dijkstra_type>())
                    {
                        // frontier_node has been scored before
                        if (newLen < frontier_node.template AuxData<dijkstra_type>()->shortestPathLength)
                        {
                            // update frontier_node's score
                            frontier_node.template AuxData<dijkstra_type>()->shortestPathLength = newLen;
                            frontier_node.template AuxData<dijkstra_type>()->previousVertex = current_node;

                            frontier.insert(&frontier_node);
                        }
                    }
                    else
                    {
                        // frontier_node hasn't been scored before
                        frontier_node.template AuxData<dijkstra_type>() = new dijkstra_type(
                            /*shortestPathLength*/ newLen,
                            /*previousVertex*/ current_node);

                        frontier.insert(&frontier_node);
                    }
                });
        }

        // visit the shortest path
        if (visitShortestPath && to.template AuxData<dijkstra_type>())
        {
            std::stack<vertex_type*> shortestPath;
            shortestPath.push(&to);
            while (true)
            {
                vertex_type* top = shortestPath.top();
                if (top == &from)
                    break;
                shortestPath.push(top->template AuxData<dijkstra_type>()->previousVertex);
            }

            while (!shortestPath.empty())
            {
                vertex_type* top = shortestPath.top();
                visitShortestPath(*top, top->template AuxData<dijkstra_type>()->shortestPathLength);
                shortestPath.pop();
            }
        }

        // clear Dijkstra's shortest path data
        if (clearDijkstraData)
        {
            graph.VisitVertices(
                [](vertex_type& v)
                {
                    if (v.template AuxData<dijkstra_type>())
                    {
                        delete v.template AuxData<dijkstra_type>();
                        v.template AuxData<dijkstra_type>() = nullptr;
                    }
                });
        }
    }

    /**
     * @brief Finds the minimum-length path in undirected @p graph
     * from a vertex with identifier @p from_id to every other vertex in the @p graph.
     * For each vertex in the @p graph creates a structure DijkstraShortestPath_Data (containng
     * the shortest length to this vertex and the pointer to its predecessor in the shortest path)
     * and puts a pointer to that structure to the AuxData() attribute of the vertex - this structure
     * can be accessed afterwards if you've passed false as @p clearDijkstraData, but in that case
     * you are responsible for deallocating memory pointed to by AuxData() in each vertex.
     * Calls functor @p visitShortestPath (unless it is null) for each vertex in the path
     * from the vertex with identifier @p from_id to a vertex with identifier @p to_id.
     *
     * @see DijkstraShortestPath_Data
     *
     * @param graph
     * @param from_id - identifier of a vertex which is the beginning of all shortest paths to be found.
     * @param to_id - identifier of a vertex which is the destination of the shortest path visited with @p visitShortestPath functor.
     * @param visitShortestPath - functor accepting a reference to vertex an the length of the shortest path from the
     * vertex with identifier @p @from_id.
     * @param clearDijkstraData - if true, clears all DijkstraShortestPath_Data structures created during the algorithm.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void DijkstraShortestPath_Undirected(
        Graph<TId, TLen>& graph,
        TId from_id,
        TId to_id,
        std::function<void(Vertex<TId, TLen>& /*vertex*/, TLen /*shortestPathLength*/)> visitShortestPath,
        bool clearDijkstraData = true)
    {
        using vertex_type = Vertex<TId, TLen>;
        using dijkstra_type = DijkstraShortestPath_Data<TId, TLen>;

        vertex_type& from = graph.GetVertexById(from_id);
        vertex_type& to = graph.GetVertexById(to_id);

        from.template AuxData<dijkstra_type>() = new dijkstra_type;

        DijkstraShortestPath_VertexMinHeap<TId, TLen> frontier;
        frontier.insert(&from);

        // Score each vertex in the graph
        while (!frontier.empty())
        {
            vertex_type* current_node = frontier.popClosestVertex();
            if (current_node->Discovered()) continue;
            current_node->Discovered() = true;

            VisitEdges<TId, TLen>(
                /*vertex*/ *current_node,
                /*visitor*/
                [current_node, &frontier](vertex_type& frontier_node, TLen length)
                {
                    if (frontier_node.Discovered())
                        return;

                    TLen newLen = current_node->template AuxData<dijkstra_type>()->shortestPathLength + length;

                    if (frontier_node.template AuxData<dijkstra_type>())
                    {
                        // frontier_node has been scored before
                        if (newLen < frontier_node.template AuxData<dijkstra_type>()->shortestPathLength)
                        {
                            // update frontier_node's score
                            frontier_node.template AuxData<dijkstra_type>()->shortestPathLength = newLen;
                            frontier_node.template AuxData<dijkstra_type>()->previousVertex = current_node;

                            frontier.insert(&frontier_node);
                        }
                    }
                    else
                    {
                        // frontier_node hasn't been scored before
                        frontier_node.template AuxData<dijkstra_type>() = new dijkstra_type(
                            /*shortestPathLength*/ newLen,
                            /*previousVertex*/ current_node);

                        frontier.insert(&frontier_node);
                    }
                });
        }

        // visit the shortest path
        if (visitShortestPath && to.template AuxData<dijkstra_type>())
        {
            std::stack<vertex_type*> shortestPath;
            shortestPath.push(&to);
            while (true)
            {
                vertex_type* top = shortestPath.top();
                if (top == &from)
                    break;
                shortestPath.push(top->template AuxData<dijkstra_type>()->previousVertex);
            }

            while (!shortestPath.empty())
            {
                vertex_type* top = shortestPath.top();
                visitShortestPath(*top, top->template AuxData<dijkstra_type>()->shortestPathLength);
                shortestPath.pop();
            }
        }

        // clear Dijkstra's shortest path data
        if (clearDijkstraData)
        {
            graph.VisitVertices(
                [](vertex_type& v)
                {
                    if (v.template AuxData<dijkstra_type>())
                    {
                        delete v.template AuxData<dijkstra_type>();
                        v.template AuxData<dijkstra_type>() = nullptr;
                    }
                });
        }
    }
}
