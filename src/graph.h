#pragma once

#include "requires.h"

#include <functional>    // for std::function
#include <list>          // for std::list
#include <unordered_map> // for std::unordered_map

namespace cs
{
    template<typename TId>
    class VertexBase;

    template<typename TVertex, typename TLen>
    class Edge;

    template<typename TId, typename TLen>
    class Graph;

    template<typename TId = int>
    class VertexBase
    {
    private:
        TId id;

    public:
        VertexBase(
            TId id) :
            id(id)
        {}

        VertexBase(const VertexBase&) = delete;

    public:
        VertexBase& operator=(const VertexBase&) = delete;

        TId Id() const { return id; }
    };

    template<typename TVertex, typename TLen = int>
    class Edge
    {
    private:
        TVertex* from;
        TVertex* to;
        TLen length;

    public:
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

        TVertex& From() { return *from; }
        const TVertex& From() const { return *from; }

        TVertex& To() { return *to; }
        const TVertex& To() const { return *to; }

        TLen Length() const { return length; }
    };

    template<typename TId, typename TLen>
    class Vertex : public VertexBase<TId>
    {
        template<typename, typename>
        friend class Graph;

    public:
        using edge_type = Edge<Vertex, TLen>;

    private:
        std::list<edge_type*> incomingEdges;
        std::list<edge_type*> outgoingEdges;
        bool discovered; // used in many algorithms (breadth-first search, depth-first search, Dijkstra's shortest path etc.)

    public:
        Vertex(
            TId id) :
            VertexBase<TId>(id),
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

    template<typename TId = int, typename TLen = int>
    class Graph
    {
    public:
        using vertex_type = Vertex<TId, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

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
                
                for (auto key_value : vertices)
                    delete key_value.second;

                for (edge_type* edge : edges)
                    delete edge;
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
            if (!result.second)
            {
                delete vertex;
                return nullptr;
            }
            return vertex;
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
}
