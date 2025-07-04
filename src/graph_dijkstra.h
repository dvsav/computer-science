#pragma once

#include "graph.h"    // for cs::Graph, cs::Vertex, cs::Edge

#include <functional> // for std::function
#include <queue>      // std::priority_queue
#include <stack>      // for std::stack

namespace cs
{
    /**
     * @class DijkstraShortestPath_Data.
     * @brief This class represents data produced by Dijkstra's shortest path algorithm
     * and associated with each vertex of a graph. The data include the Dijkstra's
     * greedy score (shortest path length) as well as the pointer to the previous vertex
     * in the shortest path.
     */
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
     * @brief Finds the minimum-length path in a directed @p graph
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
        using edge_type = Edge<vertex_type, TLen>;
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
                [current_node, &frontier](vertex_type& frontier_node, edge_type& edge)
                {
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
}
