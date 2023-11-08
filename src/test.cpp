#include "graph.h"            // for cs::Graph, cs::Vertex, cs::Edge
#include "graph_algorithms.h" // for cs::BreadthFirstSearch_Directed, cs::DepthFirstSearch_Directed, ...
#include "merge_sort.h"       // for cs::merge_sort
#include "print.h"            // for cs::print
#include "quick_sort.h"       // for cs::quick_sort_lomuto_partition, cs::quick_sort_randomized_partition
#include "utility.h"          // for cs::ReverseComparator, cs::files_textually_equal

#include <algorithm>          // for std::find
#include <array>              // for std::array
#include <iostream>           // for std:::cout
#include <vector>             // for std::vector

// Catch2 - a single header unit test framework
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "Catch2/catch.hpp"

TEST_CASE("Vectors are sorted", "[sort]")
{
    std::vector<int> vec{ 5, 2, 8, 15, 48, 1, -6, 7, 3, 8, -10 };
    std::vector<int> ascend_vec{ -10, -6, 1, 2, 3, 5, 7, 8, 8, 15, 48 };
    std::vector<int> descen_vec{ 48, 15, 8, 8, 7, 5, 3, 2, 1, -6, -10 };

    SECTION("merge_sort in ascending order")
    {
        cs::merge_sort(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("merge_sort in descending order")
    {
        cs::merge_sort<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("quick_sort_lomuto_partition in ascending order")
    {
        cs::quick_sort_lomuto_partition(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("quick_sort_lomuto_partition in descending order")
    {
        cs::quick_sort_lomuto_partition<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("quick_sort_randomized_partition in ascending order")
    {
        cs::quick_sort_randomized_partition(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("quick_sort_randomized_partition in descending order")
    {
        cs::quick_sort_randomized_partition<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }
}

TEST_CASE("Graph algorithms", "[graph]")
{
    using graph_type = cs::Graph<>;
    using vertex_type = typename graph_type::vertex_type;
    using id_type = graph_type::id_type;
    using length_type = graph_type::length_type;

    const id_type vertices_number = 10;
    graph_type g;
    for (id_type i = 0; i < vertices_number; i++)
        g.AddVertex(/*id*/ i);

    /*
       1 - 4
      / \
     0 - 2 - 5
      \   \
       3   6 - 7 - 8 - 9
    */

    g.AddEdge(/*from_id*/ 0, /*to_id*/ 1);
    g.AddEdge(/*from_id*/ 0, /*to_id*/ 2);
    g.AddEdge(/*from_id*/ 0, /*to_id*/ 3);
    g.AddEdge(/*from_id*/ 1, /*to_id*/ 2);
    g.AddEdge(/*from_id*/ 1, /*to_id*/ 4);
    g.AddEdge(/*from_id*/ 2, /*to_id*/ 5);
    g.AddEdge(/*from_id*/ 2, /*to_id*/ 6);
    g.AddEdge(/*from_id*/ 6, /*to_id*/ 7);
    g.AddEdge(/*from_id*/ 7, /*to_id*/ 8);
    g.AddEdge(/*from_id*/ 8, /*to_id*/ 9);

    SECTION("check that graph constructed correctly")
    {
        REQUIRE(g.EdgesNumber() == 10);
        REQUIRE(g.VerticesNumber() == vertices_number);

        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(1)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(2)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(3)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(1), /*to*/ g.GetVertexById(2)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(1), /*to*/ g.GetVertexById(4)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(2), /*to*/ g.GetVertexById(5)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(2), /*to*/ g.GetVertexById(6)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(6), /*to*/ g.GetVertexById(7)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(7), /*to*/ g.GetVertexById(8)));
        REQUIRE(cs::DirectedEdgeExists(/*from*/ g.GetVertexById(8), /*to*/ g.GetVertexById(9)));

        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(1)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(2)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(3)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(1), /*b*/ g.GetVertexById(2)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(1), /*b*/ g.GetVertexById(4)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(2), /*b*/ g.GetVertexById(5)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(2), /*b*/ g.GetVertexById(6)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(6), /*b*/ g.GetVertexById(7)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(7), /*b*/ g.GetVertexById(8)));
        REQUIRE(cs::UndirectedEdgeExists(/*a*/ g.GetVertexById(8), /*b*/ g.GetVertexById(9)));

        REQUIRE(g.GetVertexById(0).NumberOfOutgoingEdges() == 3);
        REQUIRE(g.GetVertexById(0).NumberOfIncomingEdges() == 0);

        REQUIRE(g.GetVertexById(1).NumberOfOutgoingEdges() == 2);
        REQUIRE(g.GetVertexById(1).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(2).NumberOfOutgoingEdges() == 2);
        REQUIRE(g.GetVertexById(2).NumberOfIncomingEdges() == 2);

        REQUIRE(g.GetVertexById(3).NumberOfOutgoingEdges() == 0);
        REQUIRE(g.GetVertexById(3).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(4).NumberOfOutgoingEdges() == 0);
        REQUIRE(g.GetVertexById(4).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(5).NumberOfOutgoingEdges() == 0);
        REQUIRE(g.GetVertexById(5).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(6).NumberOfOutgoingEdges() == 1);
        REQUIRE(g.GetVertexById(6).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(7).NumberOfOutgoingEdges() == 1);
        REQUIRE(g.GetVertexById(7).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(8).NumberOfOutgoingEdges() == 1);
        REQUIRE(g.GetVertexById(8).NumberOfIncomingEdges() == 1);

        REQUIRE(g.GetVertexById(9).NumberOfOutgoingEdges() == 0);
        REQUIRE(g.GetVertexById(9).NumberOfIncomingEdges() == 1);
    }

    SECTION("add and remove a vertex")
    {
        g.AddVertex(/*id*/ 10);
        REQUIRE(g.VerticesNumber() == vertices_number + 1);

        g.AddEdge(/*from_id*/ 10, /*to_id*/ 9);
        REQUIRE(g.EdgesNumber() == 11);

        g.AddEdge(/*from_id*/ 8, /*to_id*/ 10);
        REQUIRE(g.EdgesNumber() == 12);

        g.RemoveVertex(/*id*/ 10);
        REQUIRE(g.VerticesNumber() == vertices_number);
        REQUIRE(g.EdgesNumber() == 10);
    }

    SECTION("breadth-first search (undirected graph)")
    {
        std::vector<id_type> search_order;
        search_order.reserve(g.VerticesNumber());

        cs::BreadthFirstSearch_Undirected<id_type, length_type>(
            /*graph*/ g,
            /*root_id*/ 0,
            /*visit*/
            [&search_order](vertex_type& v) -> void
            {
                search_order.push_back(v.Id());
            });

        // check that all graph vertices are present in the search result
        REQUIRE(search_order.size() == g.VerticesNumber());
        g.VisitVertices(
            [&search_order](const vertex_type& v)
            {
                REQUIRE(std::find(search_order.begin(), search_order.end(), v.Id()) != search_order.end());
            });
    }

    SECTION("breadth-first search (directed graph)")
    {
        std::vector<id_type> search_order;
        search_order.reserve(g.VerticesNumber());

        cs::BreadthFirstSearch_Directed<id_type, length_type>(
            /*graph*/ g,
            /*root_id*/ 0,
            /*visit*/
            [&search_order](vertex_type& v) -> void
            {
                search_order.push_back(v.Id());
            });

        // check that all graph vertices are present in the search result
        REQUIRE(search_order.size() == g.VerticesNumber());
        g.VisitVertices(
            [&search_order](const vertex_type& v)
            {
                REQUIRE(std::find(search_order.begin(), search_order.end(), v.Id()) != search_order.end());
            });
    }

    SECTION("depth-first search (undirected graph)")
    {
        std::vector<id_type> search_order;
        search_order.reserve(g.VerticesNumber());

        cs::DepthFirstSearch_Undirected<id_type, length_type>(
            /*graph*/ g,
            /*root_id*/ 0,
            /*visit*/
            [&search_order](vertex_type& v) -> void
            {
                search_order.push_back(v.Id());
            });

        // check that all graph vertices are present in the search result
        REQUIRE(search_order.size() == g.VerticesNumber());
        g.VisitVertices(
            [&search_order](const vertex_type& v)
            {
                REQUIRE(std::find(search_order.begin(), search_order.end(), v.Id()) != search_order.end());
            });
    }

    SECTION("depth-first search (directed graph)")
    {
        std::vector<id_type> search_order;
        search_order.reserve(g.VerticesNumber());

        cs::DepthFirstSearch_Directed<id_type, length_type>(
            /*graph*/ g,
            /*root_id*/ 0,
            /*visit*/
            [&search_order](vertex_type& v) -> void
            {
                search_order.push_back(v.Id());
            });

        // check that all graph vertices are present in the search result
        REQUIRE(search_order.size() == g.VerticesNumber());
        g.VisitVertices(
            [&search_order](const vertex_type& v)
            {
                REQUIRE(std::find(search_order.begin(), search_order.end(), v.Id()) != search_order.end());
            });
    }

    SECTION("sort vertices in topological order")
    {
        std::vector<vertex_type*> topological_order;
        topological_order.reserve(g.VerticesNumber());

        cs::TopologicalSort<id_type, length_type>(
            /*graph*/ g,
            /*visit*/
            [&topological_order](vertex_type& v)
            {
                topological_order.push_back(&v);
            });

        REQUIRE(cs::IsTopologicalOrder<id_type, length_type>(topological_order.begin(), topological_order.end()));
        REQUIRE(!cs::IsTopologicalOrder<id_type, length_type>(topological_order.rbegin(), topological_order.rend()));
    }

    SECTION("find strongly connected components via Kosaraju's algorithm")
    {
        cs::VisitStronglyConnectedComponents_Kosaraju<id_type, length_type>(
            /*graph*/ g,
            /*visit*/
            [](vertex_type& v, id_type scc_id)
            {
                REQUIRE(v.Id() == scc_id);
            }
        );
    }
}