#pragma once

#include <functional> // for std::function
#include <list>       // for std::list

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

    template<typename TId, typename TData>
    class Vertex : public VertexBase<TId, TData>
    {
        template<typename TId, typename TData, typename TLen>
        friend class Graph;

    private:
        std::list<Vertex*> incomingNeighbors;
        std::list<Vertex*> outgoingNeighbors;

    public:
        Vertex(
            TId id,
            TData data) :
            VertexBase<TId, TData>(id, data),
            incomingNeighbors(),
            outgoingNeighbors()
        {}

        Vertex(const Vertex&) = delete;

    private:
        Vertex& operator=(const Vertex&) = delete;

        void AddIncomingNeighbor(Vertex* neighbor) { incomingNeighbors.push_back(neighbor); }
        void AddOutgoingNeighbor(Vertex* neighbor) { outgoingNeighbors.push_back(neighbor); }
        void RemoveIncomingNeighbor(Vertex* neighbor) { incomingNeighbors.remove(neighbor); }
        void RemoveOutgoingNeighbor(Vertex* neighbor) { outgoingNeighbors.remove(neighbor); }

    public:
        bool VisitIncomingNeighbors(std::function<bool(Vertex*)> visitor)
        {
            for (Vertex* neighbor : incomingNeighbors)
            {
                if (visitor(neighbor))
                    return true;
            }
            return false;
        }

        bool VisitIncomingNeighbors(std::function<bool(const Vertex*)> visitor) const
        {
            for (const Vertex* neighbor : incomingNeighbors)
            {
                if (visitor(neighbor))
                    return true;
            }
            return false;
        }

        bool VisitOutgoingNeighbors(std::function<bool(Vertex*)> visitor)
        {
            for (Vertex* neighbor : outgoingNeighbors)
            {
                if (visitor(neighbor))
                    return true;
            }
            return false;
        }

        bool VisitOutgoingNeighbors(std::function<bool(const Vertex*)> visitor) const
        {
            for (const Vertex* neighbor : outgoingNeighbors)
            {
                if (visitor(neighbor))
                    return true;
            }
            return false;
        }
    };

    template<typename TVertex, typename TLen>
    class Edge
    {
    private:
        TVertex* head;
        TVertex* tail;
        TLen length;

    public:
        Edge(
            TVertex* head,
            TVertex* tail,
            TLen length) :
            head(head),
            tail(tail),
            length(length)
        {}

        Edge(const Edge&) = default;

    public:
        Edge& operator=(const Edge&) = default;

        TVertex* Head() { return head; }
        const TVertex* Head() const { return head; }

        TVertex* Tail() { return tail; }
        const TVertex* Tail() const { return tail; }

        TLen Length() const { return length; }
    };

    template<typename TId, typename TData, typename TLen>
    class Graph
    {
    public:
        using vertex_type = Vertex<TId, TData>;
        using edge_type = Edge<vertex_type, TLen>;

    private:
        std::list<vertex_type*> vertices; // linked list doesn't preserve memory addresses of its elements, which is why we cannot use std::list<vertex_type>
        std::list<edge_type*> edges;      // linked list doesn't preserve memory addresses of its elements, which is why we cannot use std::list<edge_type>
        bool is_disposed;

    public:
        Graph() :
            vertices(),
            edges(),
            is_disposed(false)
        {}

        Graph(Graph&& rvalue) :
            vertices(std::move(rvalue.vertices)),
            edges(std::move(rvalue.edges))
        {
            rvalue.is_disposed = true;
        }

        Graph(const Graph&) = delete;

        ~Graph()
        {
            if (!is_disposed)
            {
                is_disposed = true;
                
                for (vertex_type* vertex : vertices)
                    delete vertex;

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
            vertices.push_back(vertex);
            return vertex;
        }

        edge_type* AddEdge(vertex_type* head, vertex_type* tail, TLen length)
        {
            edge_type* edge = new edge_type(head, tail, length);
            head->AddOutgoingNeighbor(tail);
            tail->AddIncomingNeighbor(head);
            edges.push_back(edge);
            return edge;
        }

        bool VisitVertices(std::function<bool(vertex_type*)> visitor)
        {
            for (vertex_type* vertex : vertices)
            {
                if (visitor(vertex))
                    return true;
            }
            return false;
        }

        bool VisitVertices(std::function<bool(const vertex_type*)> visitor) const
        {
            for (const vertex_type* vertex : vertices)
            {
                if (visitor(vertex))
                    return true;
            }
            return false;
        }

        bool VisitEdges(std::function<bool(edge_type*)> visitor)
        {
            for (edge_type* edge : vertices)
            {
                if (visitor(edge))
                    return true;
            }
            return false;
        }

        bool VisitEdges(std::function<bool(const edge_type*)> visitor) const
        {
            for (const edge_type* edge : vertices)
            {
                if (visitor(edge))
                    return true;
            }
            return false;
        }
    };
}
