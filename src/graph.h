#pragma once

#include "requires.h"

#include <functional>    // for std::function, std::hash
#include <list>          // for std::list
#include <unordered_map> // for std::unordered_map
#include <utility>       // for std::pair

namespace cs
{
    template<typename TId, typename TData>
    class VertexBase;

    template<typename TVertex, typename TLen>
    class Edge;

    template<typename TId, typename TData, typename TLen>
    class Graph;

    template<typename TId, typename TData>
    class VertexBase
    {
    private:
        TId id;
        TData data;

    public:
        VertexBase(
            TId id,
            TData data) :
            id(id),
            data(data)
        {}

        VertexBase(const VertexBase&) = delete;

    public:
        VertexBase& operator=(const VertexBase&) = delete;

        TId Id() const { return id; }

        TData& Data() { return data; }
        const TData& Data() const { return data; }
    };

    template<typename TVertex, typename TLen>
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
            TLen length) :
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

    template<typename TId, typename TData, typename TLen>
    class Vertex : public VertexBase<TId, TData>
    {
        template<typename, typename, typename>
        friend class Graph;

    public:
        using edge_type = Edge<Vertex, TLen>;

    private:
        std::list<edge_type*> incomingEdges;
        std::list<edge_type*> outgoingEdges;
        bool discovered; // used in many algorithms (breadth-first search, depth-first search, Dijkstra's shortest path etc.)

    public:
        Vertex(
            TId id,
            TData data) :
            VertexBase<TId, TData>(id, data),
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

    template<typename TId, typename TData, typename TLen>
    void VisitInNeighbors(
        Vertex<TId, TData, TLen>& vertex,
        std::function<void(Vertex<TId, TData, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitIncomingEdges([visitor](edge_type& edge) -> void { return visitor(edge.From()); });
    }

    template<typename TId, typename TData, typename TLen>
    void VisitOutNeighbors(
        Vertex<TId, TData, TLen>& vertex,
        std::function<void(Vertex<TId, TData, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        vertex.VisitOutgoingEdges([visitor](edge_type& edge) -> void { return visitor(edge.To()); });
    }

    template<typename TId, typename TData, typename TLen>
    void VisitNeighbors(
        Vertex<TId, TData, TLen>& vertex,
        std::function<void(Vertex<TId, TData, TLen>&)> visitor)
    {
        VisitInNeighbors<TId, TData, TLen>(vertex, visitor);
        VisitOutNeighbors<TId, TData, TLen>(vertex, visitor);
    }


    template<typename TId, typename TData, typename TLen>
    class Graph
    {
    public:
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

    private:
        std::unordered_map<TId, vertex_type*> vertices; // unordered_map doesn't preserve memory addresses of its elements, which is why we cannot use std::unordered_map<vertex_type>

        struct PairHash
        {
            std::size_t operator () (const std::pair<TId, TId>& p) const
            {
                // Combine the hashes of the two elements using bit shift and birwise xor
                return std::hash<TId>{}(p.first) ^ (std::hash<TId>{}(p.second) << 1);
            }
        };

        std::unordered_map<std::pair<TId, TId>, edge_type*, PairHash> edges; // unordered_map doesn't preserve memory addresses of its elements, which is why we cannot use std::unordered_map<vertex_type>
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

                for (auto key_value : edges)
                    delete key_value.second;
            }
        }

    public:
        Graph& operator=(const Graph&) = delete;

        size_t VerticesNumber() const { return vertices.size(); }

        size_t EdgesNumber() const { return edges.size(); }

        vertex_type* AddVertex(TId id, TData data)
        {
            vertex_type* vertex = new vertex_type(id, data);
            auto result = vertices.insert({ id, vertex });
            if (!result.second)
            {
                delete vertex;
                return nullptr;
            }
            return vertex;
        }

        edge_type* AddEdge(TId from_id, TId to_id, TLen length)
        {
            vertex_type& from = GetVertexById(from_id);
            vertex_type& to = GetVertexById(to_id);
            edge_type* edge = new edge_type(&from, &to, length);
            from.AddOutgoingEdge(edge);
            to.AddIncomingEdge(edge);
            edges[{from_id, to_id}] = edge;
            return edge;
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
            for (auto& key_value : edges)
                visitor(*key_value.second);
        }

        void VisitEdges(std::function<void(const edge_type&)> visitor) const
        {
            for (const auto& key_value : edges)
                visitor(*key_value.second);
        }

        vertex_type& GetVertexById(TId id) { return *vertices.at(id); }

        const vertex_type& GetVertexById(TId id) const { return *vertices.at(id); }

        edge_type& GetEdgeByIds(TId from_id, TId to_id) { return *edges.at({from_id, to_id}); }

        const edge_type& GetEdgeByIds(TId from_id, TId to_id) const { return *edges.at({from_id, to_id}); }
    };

    template<typename TId, typename TData, typename TLen>
    void ClearAuxData(Graph<TId, TData, TLen>& graph)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        graph.VisitVertices(
            [](vertex_type& v)
            {
                v.ClearAuxData();
                return false;
            });
    }
}
