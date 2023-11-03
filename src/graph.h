#pragma once

#include "requires.h"

#include <functional>    // for std::function
#include <list>          // for std::list
#include <unordered_map> // for std::unordered_map

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

        union AuxiliaryData
        {
            int intValue;
            bool boolValue;
            void* voidPtr;
            Vertex* vertexPtr;

            AuxiliaryData() :
                intValue(0)
            {}

            void Clear() { intValue = 0; }
        };

    private:
        std::list<edge_type*> incomingEdges;
        std::list<edge_type*> outgoingEdges;
        AuxiliaryData auxData; // auxiliary data which may be needed for graph algorithms (e.g. bool explored in breadth-firs search algorithm)

    public:
        Vertex(
            TId id,
            TData data) :
            VertexBase<TId, TData>(id, data),
            incomingEdges(),
            outgoingEdges(),
            auxData()
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

        bool VisitIncomingEdges(std::function<bool(edge_type&)> visitor)
        {
            for (edge_type* edge : incomingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool VisitIncomingEdges(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : incomingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool VisitOutgoingEdges(std::function<bool(edge_type&)> visitor)
        {
            for (edge_type* edge : outgoingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool VisitOutgoingEdges(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : outgoingEdges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        const AuxiliaryData& AuxData() const { return auxData; }

        AuxiliaryData& AuxData() { return auxData; }
    };

    template<typename TId, typename TData, typename TLen>
    bool VisitNeighbors(
        Vertex<TId, TData, TLen>& vertex,
        std::function<bool(Vertex<TId, TData, TLen>&)> visitor)
    {
        using vertex_type = Vertex<TId, TData, TLen>;
        using edge_type = Edge<vertex_type, TLen>;

        if (vertex.VisitIncomingEdges([visitor](edge_type& edge) -> bool { return visitor(edge.From()); }))
            return true;

        if (vertex.VisitOutgoingEdges([visitor](edge_type& edge) -> bool { return visitor(edge.To()); }))
            return true;

        return false;
    }

    template<typename TId, typename TData, typename TLen>
    class Graph
    {
    public:
        using vertex_type = Vertex<TId, TData, TLen>;
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
            edges.push_back(edge);
            return edge;
        }

        bool VisitVertices(std::function<bool(vertex_type&)> visitor)
        {
            for (auto key_value : vertices)
            {
                if (visitor(*key_value.second))
                    return true;
            }
            return false;
        }

        bool VisitVertices(std::function<bool(const vertex_type&)> visitor) const
        {
            for (auto key_value : vertices)
            {
                if (visitor(*key_value.second))
                    return true;
            }
            return false;
        }

        bool VisitEdges(std::function<bool(edge_type&)> visitor)
        {
            for (edge_type* edge : edges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        bool VisitEdges(std::function<bool(const edge_type&)> visitor) const
        {
            for (const edge_type* edge : edges)
            {
                if (visitor(*edge))
                    return true;
            }
            return false;
        }

        vertex_type& GetVertexById(TId id) { return *vertices.at(id); }

        const vertex_type& GetVertexById(TId id) const { return *vertices.at(id); }
    };

    template<typename TId, typename TData, typename TLen>
    void ClearAuxData(Graph<TId, TData, TLen>& graph)
    {
        using vertex_type = Vertex<TId, TData, TLen>;

        graph.VisitVertices(
            [](vertex_type& v)
            {
                v.AuxData().Clear();
                return false;
            });
    }
}
