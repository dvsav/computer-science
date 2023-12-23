#pragma once

#include "requires.h"    // for Requires::ArgumentNotNull
#include "utility.h"     // for operator>>

#include <functional>    // for std::function
#include <list>          // for std::list
#include <iostream>      // for std::ostream, std::istream
#include <iomanip>       // for std::ws, std::noskips
#include <unordered_map> // for std::unordered_map

namespace cs
{
    template<typename TId>
    class VertexBase;

    template<typename TVertex, typename TLen>
    class Edge;

    template<typename TId, typename TLen>
    class Graph;

    /**
     * @class Edge
     * @brief Class representing an edge between two vertices of of a graph.
     *
     * An edge has a tail vertex (from) and a head vertex (to).
     * An edge can be treated as directed (from ---> to) or undirected depending on your purposes.
     * An edge has a length which may or may not be used depending on your purposes.
     *
     * @tparam TVertex The data type of vertex.
     * @tparam TLen The data type of edge length.
     */
    template<typename TVertex, typename TLen = int>
    class Edge
    {
        template <typename, typename>
        friend class Graph;

    private:
        TVertex* from;
        TVertex* to;
        TLen length;

    private:
        /**
         * @brief Creates a graph edge.
         *
         * @param from - pointer to the tail of the edge.
         * @param to - pointe to the head of the edge.
         * @param length - length of the edge.
         */
        Edge(
            TVertex* from,
            TVertex* to,
            TLen length = TLen()) :
            from(from),
            to(to),
            length(length)
        {
            Requires::ArgumentNotNull(from, NAMEOF(from), FUNCTION_INFO);
            Requires::ArgumentNotNull(to, NAMEOF(to), FUNCTION_INFO);
        }

        Edge(const Edge&) = default;

    public:
        Edge& operator=(const Edge&) = default;

        /**
         * @brief Returns a reference to the tail of the edge.
         * @return Reference to the tail of the edge.
         */
        TVertex& From() { return *from; }
        /**
         * @brief Returns a reference to the tail of the edge.
         * @return Reference to the tail of the edge.
         */
        const TVertex& From() const { return *from; }

        /**
         * @brief Returns a reference to the head of the edge.
         * @return Reference to the head of the edge.
         */
        TVertex& To() { return *to; }
        /**
         * @brief Returns a reference to the head of the edge.
         * @return Reference to the head of the edge.
         */
        const TVertex& To() const { return *to; }

        /**
         * @brief Returns the length of the edge.
         * @return Length of the edge.
         */
        TLen Length() const { return length; }
    };

    /**
     * @class Vertex
     * @brief Class representing a vertex of a graph.
     *
     * Each vertex is identified by an Id.
     * The class stores a collection of incoming and outgoing edges.
     * It also includes 'discovered' boolean flag used in many graph algorithms.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    class Vertex
    {
        template <typename, typename>
        friend class Graph;

    public:
        /**
         * @brief The type of id which uniquely identifies a vertex.
         */
        using id_type = TId;

        /**
        * @brief The type of edge between two vertices.
        */
        using edge_type = Edge<Vertex, TLen>;

    private:
        TId id;
        std::list<edge_type*> incomingEdges;
        std::list<edge_type*> outgoingEdges;
        bool discovered;
        void* auxData;

    private:
        Vertex(
            TId id) :
            id(id),
            incomingEdges(),
            outgoingEdges(),
            discovered(false),
            auxData(nullptr)
        {}

        Vertex(const Vertex&) = delete;

    private:
        Vertex& operator=(const Vertex&) = delete;

        void AddIncomingEdge(edge_type* edge) { incomingEdges.push_back(edge); }
        void AddOutgoingEdge(edge_type* edge) { outgoingEdges.push_back(edge); }

        void RemoveIncomingEdge(edge_type* edge) { incomingEdges.remove(edge); }
        void RemoveOutgoingEdge(edge_type* edge) { outgoingEdges.remove(edge); }

    public:
        /**
         * @brief Vertex id, is supposed to uniquely identify a vertex.
         */
        TId Id() const { return id; }

        /**
         * @brief The number of incoming edges. Incoming edge of a vertex is an edge whose head points to this vertex.
         * Differentiation between incoming and ougoing edges makes sense only in directed graphs.
         * In undirected graph incoming and outgoing edges can be treated as two parts of a larger collection containing all edges.
         */
        size_t NumberOfIncomingEdges() const { return incomingEdges.size(); }

        /**
         * @brief The number of outgoing edges. Outgoing edge of a vertex is an edge for which this vertex is a tail.
         * Differentiation between incoming and ougoing edges makes sense only in directed graphs.
         * In undirected graph incoming and outgoing edges can be treated as two parts of a larger collection containing all edges.
         */
        size_t NumberOfOutgoingEdges() const { return outgoingEdges.size(); }

        /**
         * @brief Calls function @p visitor for each of incoming edges.
         * @param visitor - functor called for each incoming edge, accepts a reference to that edge as a parameter.
         */
        void VisitIncomingEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : incomingEdges)
                visitor(*edge);
        }

        /**
         * @brief Calls function @p visitor for each of incoming edges.
         * @param visitor - functor called for each incoming edge, accepts a reference to that edge as a parameter.
         */
        void VisitIncomingEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : incomingEdges)
                visitor(*edge);
        }

        /**
         * @brief Calls function @p visitor for each of outgoing edges.
         * @param visitor - functor called for each outgoing edge, accepts a reference to that edge as a parameter.
         */
        void VisitOutgoingEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : outgoingEdges)
                visitor(*edge);
        }

        /**
         * @brief Calls function @p visitor for each of outgoing edges.
         * @param visitor - functor called for each outgoing edge, accepts a reference to that edge as a parameter.
         */
        void VisitOutgoingEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : outgoingEdges)
                visitor(*edge);
        }

        /**
         * @brief Calls function @p visitor for each of incoming edges. Iterates over the incoming edges until either
         * all the edges have been visited or a @p visitor has returned true. In the latter case the function itself returns true.
         * @param visitor - functor called for each incoming edge, accepts a reference to that edge as a parameter, returns true if
         * the desired edge has been found and the search should be stopped.
         * @return true if a @p visitor has ever returned true, false - otherwise
         */
        bool FindIncomingEdge(std::function<bool(edge_type&)> visitor)
        {
            for (edge_type* edge : incomingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        /**
         * @brief Calls function @p visitor for each of incoming edges. Iterates over the incoming edges until either
         * all the edges have been visited or a @p visitor has returned true. In the latter case the function itself returns true.
         * @param visitor - functor called for each incoming edge, accepts a reference to that edge as a parameter, returns true if
         * the desired edge has been found and the search should be stopped.
         * @return true if a @p visitor has ever returned true, false - otherwise
         */
        bool FindIncomingEdge(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : incomingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        /**
         * @brief Calls function @p visitor for each of outgoing edges. Iterates over the outgoing edges until either
         * all the edges have been visited or a @p visitor has returned true. In the latter case the function itself returns true.
         * @param visitor - functor called for each outgoing edge, accepts a reference to that edge as a parameter, returns true if
         * the desired edge has been found and the search should be stopped.
         * @return true if a @p visitor has ever returned true, false - otherwise
         */
        bool FindOutgoingEdge(std::function<bool(edge_type&)> visitor)
        {
            for (edge_type* edge : outgoingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        /**
         * @brief Calls function @p visitor for each of outgoing edges. Iterates over the outgoing edges until either
         * all the edges have been visited or a @p visitor has returned true. In the latter case the function itself returns true.
         * @param visitor - functor called for each outgoing edge, accepts a reference to that edge as a parameter, returns true if
         * the desired edge has been found and the search should be stopped.
         * @return true if a @p visitor has ever returned true, false - otherwise
         */
        bool FindOutgoingEdge(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : outgoingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        /**
         * @brief Boolean flag used by practically all graph algorithms (breadth-first search, depth-first search, Dijkstra's shortest path etc.).
         */
        bool Discovered() const { return discovered; }

        /**
         * @brief Boolean flag used by practically all graph algorithms (breadth-first search, depth-first search, Dijkstra's shortest path etc.).
         */
        bool& Discovered() { return discovered; }

        /**
         * @brief Pointer to auxiliary data allowing graph algorithms to store (most probably - temporarily) arbitrary data (depending on a particular algorithm).
         * For example, Dijkstra's shortest path algorithm may store there the Dijkstra's greedy score and a pointer to previous vertex).
         */
        template<typename T>
        const T* AuxData() const { return reinterpret_cast<const T*>(auxData); }

        /**
         * @brief Pointer to auxiliary data allowing graph algorithms to store (most probably - temporarily) arbitrary data (depending on a particular algorithm).
         * For example, Dijkstra's shortest path algorithm may store there the Dijkstra's greedy score and a pointer to previous vertex).
         */
        template<typename T>
        T*& AuxData() { return *reinterpret_cast<T**>(&auxData); }
    };

    /**
     * @brief Calls @p visitor functor for all in-neighbors of @p vertex.
     * @p vertex is connected with an in-neighbor by an incoming edge.
     * 
     * @param vertex
     * @param visitor - functor called for each in-neghbor
     * 
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitInNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitIncomingEdges([visitor](edge_type& edge) -> void { return visitor(edge.From()); });
    }

    /**
     * @brief Calls @p visitor functor for all in-neighbors of @p vertex.
     * @p vertex is connected with an in-neighbor by an incoming edge.
     *
     * @param vertex
     * @param visitor - functor called for each in-neghbor
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitInNeighbors(
        const Vertex<TId, TLen>& vertex,
        std::function<void(const Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitIncomingEdges([visitor](const edge_type& edge) -> void { return visitor(edge.From()); });
    }

    /**
     * @brief Calls @p visitor functor for all out-neighbors of @p vertex.
     * @p vertex is connected with an out-neighbor by an outgoing edge.
     *
     * @param vertex
     * @param visitor - functor called for each out-neghbor
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitOutNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitOutgoingEdges([visitor](edge_type& edge) -> void { return visitor(edge.To()); });
    }

    /**
     * @brief Calls @p visitor functor for all out-neighbors of @p vertex.
     * @p vertex is connected with an out-neighbor by an outgoing edge.
     *
     * @param vertex
     * @param visitor - functor called for each out-neghbor
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitOutNeighbors(
        const Vertex<TId, TLen>& vertex,
        std::function<void(const Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitOutgoingEdges([visitor](const edge_type& edge) -> void { return visitor(edge.To()); });
    }

    /**
     * @brief Calls @p visitor functor for all neighbors of @p vertex.
     * @p vertex is connected with a neighbor by an edge.
     *
     * @param vertex
     * @param visitor - functor called for each neghbor
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        VisitInNeighbors<TId, TLen>(vertex, visitor);
        VisitOutNeighbors<TId, TLen>(vertex, visitor);
    }

    /**
     * @brief Calls @p visitor functor for all neighbors of @p vertex.
     * @p vertex is connected with a neighbor by an edge.
     *
     * @param vertex
     * @param visitor - functor called for each neghbor
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitNeighbors(
        const Vertex<TId, TLen>& vertex,
        std::function<void(const Vertex<TId, TLen>&)> visitor)
    {
        VisitInNeighbors<TId, TLen>(vertex, visitor);
        VisitOutNeighbors<TId, TLen>(vertex, visitor);
    }

    /**
     * @brief Calls @p visitor functor for all edges of @p vertex.
     *
     * @param vertex
     * @param visitor - functor called for each edge; accepts a reference to
     * a @p vertex's neighbor and a reference to an edge.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitEdges(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>& /*neighbor*/, Edge<TId, TLen>& /*edge*/)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = typename vertex_type::edge_type;

        vertex.VisitIncomingEdges(
            /*visitor*/
            [visitor](edge_type& edge)
            {
                visitor(edge.From(), edge);
            }
        );

        vertex.VisitOutgoingEdges(
            /*visitor*/
            [visitor](edge_type& edge)
            {
                visitor(edge.To(), edge);
            }
        );
    }

    /**
     * @brief Calls @p visitor functor for all edges of @p vertex.
     *
     * @param vertex
     * @param visitor - functor called for each edge; accepts a reference to
     * a @p vertex's neighbor and a reference to an edge.
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void VisitEdges(
        const Vertex<TId, TLen>& vertex,
        std::function<void(const Vertex<TId, TLen>& /*neighbor*/, const Edge<TId, TLen>& /*edge*/)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = typename vertex_type::edge_type;

        vertex.VisitIncomingEdges(
            /*visitor*/
            [visitor](const edge_type& edge)
            {
                visitor(edge.From(), edge);
            }
        );

        vertex.VisitOutgoingEdges(
            /*visitor*/
            [visitor](const edge_type& edge)
            {
                visitor(edge.To(), edge);
            }
        );
    }

    /**
     * @brief Returns the total number of edges for a given @p vertex.
     *
     * @param vertex
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     * 
     * @return number of edges
     */
    template<typename TId, typename TLen>
    size_t NumberOfEdges(Vertex<TId, TLen>& vertex)
    {
        return vertex.NumberOfIncomingEdges() + vertex.NumberOfOutgoingEdges();
    }

    /**
     * @brief Returns a pointer to a directed edge going from
     * vertex @p from to vertex @p to if it exists and nullptr otherwise.
     *
     * @param from - tail vertex
     * @param to - head vertex
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return Pointer to edge if the edge exists, nullptr otherwise.
     */
    template<typename TId, typename TLen>
    const Edge<Vertex<TId, TLen>, TLen>* FindDirectedEdge(
        const Vertex<TId, TLen>& from,
        const Vertex<TId, TLen>& to)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        const edge_type* result = nullptr;

        if (from.NumberOfOutgoingEdges() < to.NumberOfIncomingEdges())
        {
            from.FindOutgoingEdge(
                [&to, &result](const edge_type& edge)
                {
                    if (&edge.To() == &to)
                    {
                        result = &edge;
                        return true;
                    }
                    return false;
                }
            );
        }
        else
        {
            to.FindIncomingEdge(
                [&from, &result](const edge_type& edge)
                {
                    if (&edge.From() == &from)
                    {
                        result = &edge;
                        return true;
                    }
                    return false;
                }
            );
        }

        return result;
    }

    /**
     * @brief Returns a pointer to an undirected edge between
     * vertices @p a and @p b if it exists and nullptr otherwise.
     *
     * @param a - a vertex
     * @param b - a vertex
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return Pointer to edge if the edge exists, nullptr otherwise.
     */
    template<typename TId, typename TLen>
    const Edge<Vertex<TId, TLen>, TLen>* FindUndirectedEdge(
        const Vertex<TId, TLen>& a,
        const Vertex<TId, TLen>& b)
    {
        const Edge<Vertex<TId, TLen>, TLen>* edge = FindDirectedEdge(a, b);
        if (edge) return edge;
        return FindDirectedEdge(b, a);
    }

    /**
     * @class Graph
     * @brief Class representing a graph.
     * Implemented as adjacency list + edge list.
     *
     * A graph consists of a set of vertices connected by edges.
     * Graph can be treated as directed or undirected,
     * weighted (edges have lengths) or unweighted
     * (edges don't have length or it doesn't matter),
     * depending on your purposes.
     *
     * @tparam TId The data type of Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId = int, typename TLen = int>
    class Graph final
    {
    public:
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;
        using id_type = TId;
        using length_type = TLen;

    private:
        std::unordered_map<TId, vertex_type*> vertices; // unordered_map doesn't preserve memory addresses of its elements, which is why we cannot use std::unordered_map<vertex_type>
        std::list<edge_type*> edges;                    // linked list doesn't preserve memory addresses of its elements, which is why we cannot use std::list<edge_type>
        bool is_disposed;

    public:
        /**
         * @brief Creates an empty graph
         */
        Graph() :
            vertices(),
            edges(),
            is_disposed(false)
        {}

        /**
         * @brief Move constructor
         */
        Graph(Graph&& rvalue) :
            vertices(std::move(rvalue.vertices)),
            edges(std::move(rvalue.edges)),
            is_disposed(false)
        {
            rvalue.is_disposed = true;
        }

        Graph(const Graph&) = delete;

        ~Graph()
        {
            if (!is_disposed)
            {
                is_disposed = true;
                Clear();
            }
        }

    public:
        Graph& operator=(const Graph&) = delete;

        /**
         * @brief Returns the number of vertices in the graph.
         */
        size_t VerticesNumber() const { return vertices.size(); }

        /**
         * @brief Returns the number of edges in the graph.
         */
        size_t EdgesNumber() const { return edges.size(); }

        /**
         * @brief Adds a new vertex with Id = @p id to the graph.
         * @param id - vertex identifier.
         * @return Pointer to the newly created vertex.
         */
        vertex_type* AddVertex(TId id)
        {
            vertex_type* vertex = new vertex_type(id);
            auto result = vertices.insert({ id, vertex });
            if (!result.second) delete vertex;
            return result.first->second;
        }

        /**
         * @brief Removes a vertex with specified @p id from the graph.
         * @param id - vertex identifier.
         */
        void RemoveVertex(TId id)
        {
            vertex_type* vertex = vertices.at(id);

            vertex->VisitIncomingEdges(
                /*visitor*/
                [this](edge_type& in_edge)
                {
                    in_edge.From().RemoveOutgoingEdge(&in_edge);
                    edges.remove(&in_edge);
                }
            );

            vertex->VisitOutgoingEdges(
                /*visitor*/
                [this](edge_type& out_edge)
                {
                    out_edge.To().RemoveIncomingEdge(&out_edge);
                    edges.remove(&out_edge);
                }
            );

            vertices.erase(id);

            delete vertex;
        }

        /**
         * @brief Adds an edge between two vertices in the graph.
         * @param from_id - identifier of a vertex which is going to be the tail of the edge.
         * @param to_id - identifier of a vertex which is going to be the head of the edge.
         * @param length - edge length
         * @return Pointer to the newly created edge.
         */
        edge_type* AddEdge(TId from_id, TId to_id, TLen length = TLen())
        {
            vertex_type& from = GetVertexById(from_id);
            vertex_type& to = GetVertexById(to_id);
            edge_type* edge = new edge_type(&from, &to, length);
            from.AddOutgoingEdge(edge);
            to.AddIncomingEdge(edge);
            edges.push_back(edge);
            return edge;
        }

        /**
         * @brief Removes specified @p edge from the graph.
         * @param edge - reference to the edge to be removed.
         */
        void RemoveEdge(edge_type& edge)
        {
            edge.From().RemoveOutgoingEdge(&edge);
            edge.To().RemoveIncomingEdge(&edge);
            edges.remove(&edge);
        }

        /**
         * @brief Calls functor @p visitor for each vertex of the graph.
         * @param visitor - functor accepting a reference to vertex.
         */
        void VisitVertices(std::function<void(vertex_type&)> visitor)
        {
            for (auto& key_value : vertices)
                visitor(*key_value.second);
        }

        /**
         * @brief Calls functor @p visitor for each vertex of the graph.
         * @param visitor - functor accepting a reference to vertex.
         */
        void VisitVertices(std::function<void(const vertex_type&)> visitor) const
        {
            for (const auto& key_value : vertices)
                visitor(*key_value.second);
        }

        /**
         * @brief Calls functor @p predicate for each vertex of the graph until
         * the @p predicate returns true. Returns the first vertex for which the
         * @p predicate returned true.
         * @param predicate - functor accepting a reference to vertex.
         * @return the first vertex for which the @p predicate returned true.
         */
        vertex_type* FindVertex(std::function<bool(const vertex_type&)> predicate)
        {
            for (auto& key_value : vertices)
            {
                if (predicate(*key_value.second))
                    return key_value.second;
            }
            return nullptr;
        }

        /**
         * @brief Calls functor @p predicate for each vertex of the graph until
         * the @p predicate returns true. Returns the first vertex for which the
         * @p predicate returned true.
         * @param predicate - functor accepting a reference to vertex.
         * @return the first vertex for which the @p predicate returned true.
         */
        const vertex_type* FindVertex(std::function<bool(const vertex_type&)> predicate) const
        {
            for (const auto& key_value : vertices)
            {
                if (predicate(*key_value.second))
                    return key_value.second;
            }
            return nullptr;
        }

        /**
         * @brief Calls functor @p visitor for each edge in the graph.
         * @param visitor - functor accepting a reference to edge.
         */
        void VisitEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : edges)
                visitor(*edge);
        }

        /**
         * @brief Calls functor @p visitor for each edge in the graph.
         * @param visitor - functor accepting a reference to edge.
         */
        void VisitEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : edges)
                visitor(*edge);
        }

        /**
         * @brief Returns reference to a vertex with specified identifier @p id.
         * @param id - vertex identifier.
         * @return Reference to vertex.
         */
        vertex_type& GetVertexById(TId id) { return *vertices.at(id); }

        /**
         * @brief Returns reference to a vertex with specified identifier @p id.
         * @param id - vertex identifier.
         * @return Reference to vertex.
         */
        const vertex_type& GetVertexById(TId id) const { return *vertices.at(id); }

        /**
         * @brief Removes all edges and vertices from the graph.
         */
        void Clear()
        {
            for (auto key_value : vertices)
                delete key_value.second;
            vertices.clear();

            for (edge_type* edge : edges)
                delete edge;
            edges.clear();
        }
    };

    /**
     * @brief Resets to false the 'Discovered' flags of all vertices of a @p graph.
     *
     * @param graph
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen>
    void ClearDiscovered(Graph<TId, TLen>& graph)
    {
        using vertex_type = Vertex<TId, TLen>;

        graph.VisitVertices(
            [](vertex_type& v)
            {
                v.Discovered() = false;
                return false;
            });
    }

    /**
     * @brief Writes @p graph to an output stream @p os in 'adjacency list' format
     * meaning that each vertex is written along with its out-neighbors (edge length information is not written).
     *
     * @param os - output stream
     * @param graph
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     * 
     * @return reference to @p os
     */
    template<typename TId, typename TLen>
    std::ostream& write_adjacency_list(std::ostream& os, const cs::Graph<TId, TLen>& graph)
    {
        using vertex_type = cs::Vertex<TId, TLen>;

        os << '{' << std::endl;
        graph.VisitVertices(
            [&os](const vertex_type& v)
            {
                os << v.Id();
                cs::VisitOutNeighbors<TId, TLen>(
                    v,
                    [&os](const vertex_type& u)
                    {
                        os << ' ' << u.Id();
                    }
                );
                os << std::endl;
            }
        );
        os << '}' << std::endl;

        return os;
    }

    /**
     * @brief Reads @p graph from an input stream @p is in 'adjacency list' format
     * meaning that each vertex is read along with its out-neighbors (edge length information is not read,
     * so all edges get the default length value).
     *
     * @param is - input stream
     * @param graph
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return reference to @p is
     */
    template<typename TId, typename TLen>
    std::istream& read_adjacency_list(std::istream& is, cs::Graph<TId, TLen>& graph)
    {
        graph.Clear();

        while (skip_comment(is, "#")) {};

        char ch{};
        if (!(is >> ch))
            return is;

        // The fist non-whitespace character must be '{'
        // (why? - because we have the indicator of the end of input, see below)
        if (ch != '{')
        {
            is.unget();
            is.clear(std::ios_base::failbit);
            return is;
        }

        while (true)
        {
            std::vector<TId> adjacency_list;
            is >> adjacency_list;

            if (is.fail())
            {
                // The last character must be '}'
                // (why? - because we need to have the indicator of the end of input)
                is.clear();
                if (!(is >> ch)) return is;
                if (ch != '}')
                {
                    is.unget();
                    is.clear(std::ios_base::failbit);
                }
                return is;
            }

            TId vertex_id = adjacency_list[0];
            graph.AddVertex(vertex_id);
            for (auto i = adjacency_list.begin() + 1; i != adjacency_list.end(); i++)
            {
                TId neighbor_id = *i;
                graph.AddVertex(neighbor_id);
                graph.AddEdge(/*from_id*/ vertex_id, /*to_id*/ neighbor_id);
            }
        }

        return is;
    }

    /**
     * @brief Writes @p graph to an output stream @p os in 'vertex list, edge list' format
     * meaning that the list of all edges is written first and then it's followed by the list of edges.
     *
     * @param os - output stream
     * @param graph
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return reference to @p os
     */
    template<typename TId, typename TLen>
    std::ostream& write_vertex_edge_list(std::ostream& os, const cs::Graph<TId, TLen>& g)
    {
        using vertex_type = cs::Vertex<TId, TLen>;
        using edge_type = typename vertex_type::edge_type;

        os << '{' << std::endl;

        // Write vertices
        g.VisitVertices(
            [&os](const vertex_type& v)
            {
                os << v.Id() << ' ';
            }
        );
        os << std::endl;

        // Write edges
        g.VisitEdges(
            [&os](const edge_type& e)
            {
                os << e.From().Id() << ' ' << e.To().Id() << ' ' << e.Length() << std::endl;
            }
        );

        os << '}' << std::endl;

        return os;
    }

    /**
     * @brief Reads an edge from an input stream @p is in the form of
     * three variables: from (id of edge tail), to (id of edge head) and edge length.
     * The latter one is optional: if not present in the input stream it receives the default value.
     *
     * @param is - input stream
     * @param from - edge tail
     * @param to - edge head
     * @param length - edge length
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return reference to @p is
     */
    template<typename TId, typename TLen>
    std::istream& read_edge(std::istream& is, TId& from, TId& to, TLen& length)
    {
        // Skip all whitespace characters and check that the first
        // non-whitespace one is a digit
        skip_whitespace(is);
        if (!std::isdigit(is.peek()))
        {
            is.clear(std::ios_base::failbit);
            return is;
        }

        // Read the entire line until newline
        std::string line;
        std::getline(is, line);
        if (!is)
            return is;

        std::istringstream iss(line);

        // Read space separated vertex ids
        if (!(iss >> from) || !(iss >> to))
        {
            is.clear(std::ios_base::failbit);
            return is;
        }

        // The length is optional
        if (!(iss >> length))
            length = TLen{};

        return is;
    }

    /**
     * @brief Reads @p graph from an input stream @p is in 'vertex list, edge list' format
     * meaning that the list of all edges is read first and then it's followed by the list of edges.
     *
     * @param is - input stream
     * @param graph
     *
     * @tparam TId The data type of vertex Id.
     * @tparam TLen The data type of edge length.
     *
     * @return reference to @p os
     */
    template<typename TId, typename TLen>
    std::istream& read_vertex_edge_list(std::istream& is, cs::Graph<TId, TLen>& graph)
    {
        graph.Clear();

        while (skip_comment(is, "#")) {};

        char ch{};
        if (!(is >> ch))
            return is;

        // The fist non-whitespace character must be '{'
        // (why? - because we have the indicator of the end of input, see below)
        if (ch != '{')
        {
            is.unget();
            is.clear(std::ios_base::failbit);
            return is;
        }

        // Read vertices list
        std::vector<TId> vertex_list;
        is >> vertex_list;
        if (!is)
            return is;

        for (auto v : vertex_list)
            graph.AddVertex(v);

        while (true)
        {
            TId from{};
            TId to{};
            TLen length{};

            read_edge(is, from, to, length);

            if (is.fail())
            {
                // The last character must be '}'
                // (why? - because we need to have the indicator of the end of input)
                is.clear();
                if (!(is >> ch)) return is;
                if (ch != '}')
                {
                    is.unget();
                    is.clear(std::ios_base::failbit);
                }
                return is;
            }

            graph.AddEdge(from, to, length);
        }

        return is;
    }
} // namespace cs
