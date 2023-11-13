#pragma once

#include <functional>    // for std::function
#include <list>          // for std::list
#include <iostream>      // for std::ostream, std::istream
#include <iomanip>       // for std::ws, std::noskips
#include <unordered_map> // for std::unordered_map

#include "requires.h"    // for Requires::ArgumentNotNull
#include "utility.h"     // for operator>>

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
        template<typename, typename>
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
         * @brief Returns a pointer to the tail of the edge.
         * @return Pointer to the tail of the edge.
         */
        TVertex& From() { return *from; }
        /**
         * @brief Returns a pointer to the tail of the edge.
         * @return Pointer to the tail of the edge.
         */
        const TVertex& From() const { return *from; }

        /**
         * @brief Returns a pointer to the head of the edge.
         * @return Pointer to the head of the edge.
         */
        TVertex& To() { return *to; }
        /**
         * @brief Returns a pointer to the head of the edge.
         * @return Pointer to the head of the edge.
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
     * @tparam TId The data type of Id.
     * @tparam TLen The data type of edge length.
     */
    template<typename TId, typename TLen = int>
    class Vertex
    {
        template<typename, typename>
        friend class Graph;

    public:
        using edge_type = Edge<Vertex, TLen>;

    private:
        TId id;
        std::list<edge_type*> incomingEdges;
        std::list<edge_type*> outgoingEdges;
        bool discovered; // used in many algorithms (breadth-first search, depth-first search, Dijkstra's shortest path etc.)

    private:
        Vertex(
            TId id) :
            id(id),
            incomingEdges(),
            outgoingEdges(),
            discovered(false)
        {}

        Vertex(const Vertex&) = delete;

    private:
        Vertex& operator=(const Vertex&) = delete;

        void AddIncomingEdge(edge_type* edge) { incomingEdges.push_back(edge); }
        void AddOutgoingEdge(edge_type* edge) { outgoingEdges.push_back(edge); }
        void RemoveIncomingEdge(edge_type* edge) { incomingEdges.remove(edge); }
        void RemoveOutgoingEdge(edge_type* edge) { outgoingEdges.remove(edge); }

    public:
        TId Id() const { return id; }

        size_t NumberOfIncomingEdges() const { return incomingEdges.size(); }

        size_t NumberOfOutgoingEdges() const { return outgoingEdges.size(); }

        void VisitIncomingEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : incomingEdges)
                visitor(*edge);
        }

        void VisitIncomingEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : incomingEdges)
                visitor(*edge);
        }

        void VisitOutgoingEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : outgoingEdges)
                visitor(*edge);
        }

        void VisitOutgoingEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : outgoingEdges)
                visitor(*edge);
        }
        
        bool FindIncomingEdge(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : incomingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool FindOutgoingEdge(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : outgoingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool Discovered() const { return discovered; }

        bool& Discovered() { return discovered; }

        void ClearAuxData() { discovered = false; }
    };

    template<typename TId, typename TLen>
    void VisitInNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitIncomingEdges([visitor](edge_type& edge) -> void { return visitor(edge.From()); });
    }

    template<typename TId, typename TLen>
    void VisitOutNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitOutgoingEdges([visitor](edge_type& edge) -> void { return visitor(edge.To()); });
    }

    template<typename TId, typename TLen>
    void VisitOutNeighbors(
        const Vertex<TId, TLen>& vertex,
        std::function<void(const Vertex<TId, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitOutgoingEdges([visitor](const edge_type& edge) -> void { return visitor(edge.To()); });
    }

    template<typename TId, typename TLen>
    void VisitNeighbors(
        Vertex<TId, TLen>& vertex,
        std::function<void(Vertex<TId, TLen>&)> visitor)
    {
        VisitInNeighbors<TId, TLen>(vertex, visitor);
        VisitOutNeighbors<TId, TLen>(vertex, visitor);
    }

    template<typename TId, typename TLen>
    void VisitEdges(
        Vertex<TId, TLen>& vertex,
        std::function<void(Edge<Vertex<TId, TLen>, TLen>&)> visitor)
    {
        vertex.VisitIncomingEdges(visitor);
        vertex.VisitOutgoingEdges(visitor);
    }

    template<typename TId, typename TLen>
    size_t NumberOfEdges(Vertex<TId, TLen>& vertex)
    {
        return vertex.NumberOfIncomingEdges() + vertex.NumberOfOutgoingEdges();
    }

    template<typename TId, typename TLen>
    bool DirectedEdgeExists(
        const Vertex<TId, TLen>& from,
        const Vertex<TId, TLen>& to)
    {
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        return
            from.FindOutgoingEdge([&to](const edge_type& edge) { return &edge.To() == &to; }) &&
            to.FindIncomingEdge([&from](const edge_type& edge) { return &edge.From() == &from; });
    }

    template<typename TId, typename TLen>
    bool UndirectedEdgeExists(
        const Vertex<TId, TLen>& a,
        const Vertex<TId, TLen>& b)
    {
        return DirectedEdgeExists(a, b) || DirectedEdgeExists(b, a);
    }

    /**
     * @class Graph
     * @brief Class representing a graph.
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
    class Graph
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
        Graph() :
            vertices(),
            edges(),
            is_disposed(false)
        {}

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

        size_t VerticesNumber() const { return vertices.size(); }

        size_t EdgesNumber() const { return edges.size(); }

        vertex_type* AddVertex(TId id)
        {
            vertex_type* vertex = new vertex_type(id);
            auto result = vertices.insert({ id, vertex });
            if (!result.second) delete vertex;
            return result.first->second;
        }

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

        void RemoveEdge(edge_type& edge)
        {
            edge.From().RemoveOutgoingEdge(&edge);
            edge.To().RemoveIncomingEdge(&edge);
            edges.remove(&edge);
        }

        void VisitVertices(std::function<void(vertex_type&)> visitor)
        {
            for (auto key_value : vertices)
                visitor(*key_value.second);
        }

        void VisitVertices(std::function<void(const vertex_type&)> visitor) const
        {
            for (auto key_value : vertices)
                visitor(*key_value.second);
        }

        void VisitEdges(std::function<void(edge_type&)> visitor)
        {
            for (edge_type* edge : edges)
                visitor(*edge);
        }

        void VisitEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : edges)
                visitor(*edge);
        }

        vertex_type& GetVertexById(TId id) { return *vertices.at(id); }

        const vertex_type& GetVertexById(TId id) const { return *vertices.at(id); }

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

    template<typename TId, typename TLen>
    void ClearAuxData(Graph<TId, TLen>& graph)
    {
        using vertex_type = Vertex<TId, TLen>;

        graph.VisitVertices(
            [](vertex_type& v)
            {
                v.ClearAuxData();
                return false;
            });
    }

    template<typename TId, typename TLen>
    std::ostream& write_adjacency_list(std::ostream& os, const cs::Graph<TId, TLen>& g)
    {
        using vertex_type = cs::Vertex<TId, TLen>;

        os << '{' << std::endl;
        g.VisitVertices(
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

    template<typename TId, typename TLen>
    std::istream& read_adjacency_list(std::istream& is, cs::Graph<TId, TLen>& g)
    {
        g.Clear();

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
            g.AddVertex(vertex_id);
            for (auto i = adjacency_list.begin() + 1; i != adjacency_list.end(); i++)
            {
                TId neighbor_id = *i;
                g.AddVertex(neighbor_id);
                g.AddEdge(/*from_id*/ vertex_id, /*to_id*/ neighbor_id);
            }
        }

        return is;
    }

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
                os << e.From().Id() << ' ' << e.To().Id() << std::endl;
            }
        );

        os << '}' << std::endl;

        return os;
    }

    template<typename TId, typename TLen>
    std::istream& read_vertex_edge_list(std::istream& is, cs::Graph<TId, TLen>& g)
    {
        g.Clear();

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
            g.AddVertex(v);

        while (true)
        {
            std::vector<TId> edge;
            edge.reserve(2);
            is >> edge;

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

            if (edge.size() != 2)
            {
                is.clear(std::ios_base::failbit);
                return is;
            }

            g.AddEdge(edge[0], edge[1]);
        }

        return is;
    }
} // namespace cs
