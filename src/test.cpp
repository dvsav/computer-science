#include "graph.h"            // for cs::Graph
#include "graph_algorithms.h" // for cs::BreadthFirstSearch
#include "merge_sort.h"       // for cs::merge_sort
#include "print.h"            // for cs::print
#include "quick_sort.h"       // for cs::quick_sort_lomuto_partition, cs::quick_sort_randomized_partition
#include "utility.h"          // for cs::ReverseComparator, cs::files_textually_equal

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

TEST_CASE("Graph depth-first search", "[graph]")
{
    using graph_type = cs::Graph<int, int, int>;
    using vertex_type = typename graph_type::vertex_type;

    graph_type g;
    for (int i = 0; i < 10; i++)
    {
        g.AddVertex(/*id*/ i, /*data*/ 0);
    }
    REQUIRE(g.VerticesNumber() == 10);

    //   1 - 4
    //  / \
    // 0 - 2 - 5
    //  \   \
    //   3   6 - 7 - 8 - 9

    g.AddEdge(/*from_id*/ 0, /*to_id*/ 1, /*length*/ 1);
    g.AddEdge(/*from_id*/ 0, /*to_id*/ 2, /*length*/ 1);
    g.AddEdge(/*from_id*/ 0, /*to_id*/ 3, /*length*/ 1);
    g.AddEdge(/*from_id*/ 1, /*to_id*/ 2, /*length*/ 1);
    g.AddEdge(/*from_id*/ 1, /*to_id*/ 4, /*length*/ 1);
    g.AddEdge(/*from_id*/ 2, /*to_id*/ 5, /*length*/ 1);
    g.AddEdge(/*from_id*/ 2, /*to_id*/ 6, /*length*/ 1);
    g.AddEdge(/*from_id*/ 6, /*to_id*/ 7, /*length*/ 1);
    g.AddEdge(/*from_id*/ 7, /*to_id*/ 8, /*length*/ 1);
    g.AddEdge(/*from_id*/ 8, /*to_id*/ 9, /*length*/ 1);

    REQUIRE(g.EdgesNumber() == 10);

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

    // depth-first search

    std::vector<int> ethalon_search_order{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> search_order;
    search_order.reserve(10);

    cs::BreadthFirstSearch<int, int, int>(
        /*root*/ g.GetVertexById(0),
        /*visit*/
        [&search_order](vertex_type& v) -> void
        {
            search_order.push_back(v.Id());
        });

    REQUIRE(search_order == ethalon_search_order);
}