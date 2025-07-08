#pragma once

#include "graph.h"        // for cs::Graph, cs::Vertex, cs::Edge
#include "graph_search.h" // for DepthFirstSearch_Directed

#include <functional>     // for std::function
#include <iterator>       // for std::iterator_traits, std::iterator_category, std::random_access_iterator_tag
#include <type_traits>    // for std::is_same

namespace cs
{
    /**
     * @brief Calls functor @p visit for the vertices of @p graph in
     * topologically sorted order (or in reverse topologically sorted order
     * if @p visit_in_reverse_order is true). 'Topologically sorted order'
     * (suppose the sorted vertices go from left to right) assumes
     * that for each vertex v all v's incoming edges come from the left of v and
     * all v's outgoing edges go to the right of v. Note that this is applicable to
     * directed acyclic graphs only; however, this sort is also used in graphs
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
     * SCC is a subgraph such that for each vertex v in that SCC there is a path to all other
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
        // Visit the vertices of the graph in reverse-topological order
        TopologicalSort<TId, TLen>(
            /*graph*/ graph,
            /*visit*/
            [&graph, visit](Vertex<TId, TLen>& vertex)
            {
                // Since we are visiting the vertices in reverse-topological order,
                // 'vertex' is the "sink" for a single SCC, meaning that you cannot
                // reach any other SCCs from 'vertex'.
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
} // namespace cs
