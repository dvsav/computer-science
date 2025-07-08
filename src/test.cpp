#include "binary_search.h"    // for cs::BinarySearch
#include "binary_tree.h"      // for cs::BinaryTreeNode
#include "bloom_filter.h"     // for cs::BloomFilter
#include "btree.h"            // for cs::Btree
#include "graph.h"            // for cs::Graph, cs::Vertex, cs::Edge
#include "graph_algorithms.h" // for cs::BreadthFirstSearch_Directed, cs::DepthFirstSearch_Directed, ...
#include "hash_map.h"         // for cs::HashMap
#include "heap.h"             // for cs::Heap
#include "huffman_encoding.h" // for cs::HuffmanEncoding
#include "karatsuba.h"        // for cs::VeryLongInteger
#include "knapsack.h"         // for cs::PackKnapsack
#include "merge_sort.h"       // for cs::merge_sort
#include "quick_sort.h"       // for cs::quick_sort_lomuto_partition, cs::quick_sort_randomized_partition
#include "simple_sorts.h"     // for cs::selection_sort, cs::insertion_sort, cs::bubble_sort
#include "utility.h"          // for ReverseComparator, cs::files_textually_equal

#include <algorithm>          // for std::find
#include <array>              // for std::array
#include <cstdint>            // for uint8_t
#include <cstdio>             // for std::remove
#include <fstream>            // for std::ifstream, std::ofstream
#include <iostream>           // for std:::cout
#include <numeric>            // for std::accumulate
#include <string>             // for std::string
#include <utility>            // for std::pair
#include <vector>             // for std::vector

#include "nlohmann/json.hpp"  // JSON parser

// Catch2 - unit testing framework
#ifdef CATCH2_SINGLE_HEADER
    // Use the single header version of Catch2
    #define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
    #include "Catch2/catch.hpp"
#else
    // Use the static library version of Catch2
    #include <catch2/catch_test_macros.hpp>
#endif

TEST_CASE("Karatsuba", "[karatsuba]")
{
    SECTION("xy")
    {
        auto test_binary_operation = [](
            const std::string& function,
            const std::string& format,
            const std::string& lhs,
            const std::string& rhs,
            const std::string& result)
        {
            cs::VeryLongInteger x{uint8_t(0)};
            cs::VeryLongInteger y{uint8_t(0)};
            cs::VeryLongInteger z{uint8_t(0)};
            if (format == "dec")
            {
                x = cs::VeryLongInteger::FromDecimal(lhs);
                REQUIRE(x.ToDecimal() == lhs);
                y = cs::VeryLongInteger::FromDecimal(rhs);
                REQUIRE(y.ToDecimal() == rhs);
                z = cs::VeryLongInteger::FromDecimal(result);
                REQUIRE(z.ToDecimal() == result);
            }
            else if (format == "hex")
            {
                x = cs::VeryLongInteger::FromHexadecimal(lhs);
                REQUIRE(x.ToHexadecimal() == lhs);
                y = cs::VeryLongInteger::FromHexadecimal(rhs);
                REQUIRE(y.ToHexadecimal() == rhs);
                z = cs::VeryLongInteger::FromHexadecimal(result);
                REQUIRE(z.ToHexadecimal() == result);
            }
            else if (format == "bin")
            {
                x = cs::VeryLongInteger::FromBinary(lhs);
                REQUIRE(x.ToBinary() == lhs);
                y = cs::VeryLongInteger::FromBinary(rhs);
                REQUIRE(y.ToBinary() == rhs);
                z = cs::VeryLongInteger::FromBinary(result);
                REQUIRE(z.ToBinary() == result);
            }
            else
                REQUIRE(false);

            if (function == "+")
            {
                REQUIRE((x + y) == z);
            }
            else if (function == "-")
            {
                REQUIRE((x - y) == z);
            }
            else if (function == "*")
            {
                REQUIRE((x * y) == z);
                REQUIRE(cs::Karatsuba(x, y) == z);
            }
            else if (function == "/")
            {
                REQUIRE((x / y) == z);
            }
            else if (function == "&")
            {
                REQUIRE((x & y) == z);
            }
            else if (function == "|")
            {
                REQUIRE((x | y) == z);
            }
            else if (function == "^")
            {
                REQUIRE((x ^ y) == z);
            }
        };

        std::ifstream ifs("./karatsuba/very_long_integer_test_vectors.json");
        REQUIRE(ifs);
        nlohmann::json json_data = nlohmann::json::parse(ifs);
        for (const auto& test_case : json_data)
        {
            std::string function = test_case["function"];
            std::string format = test_case["format"];
            std::string lhs = test_case["args"][0];
            std::string rhs = test_case["args"][1];
            std::string result = test_case["expected"];
            //std::cout << lhs << ' ' << function << ' ' << rhs << " = " << result << std::endl;

            test_binary_operation(function, format, lhs, rhs, result);
        }
    }
}

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
        cs::merge_sort<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("quick_sort_lomuto_partition in ascending order")
    {
        cs::quick_sort_lomuto_partition(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("quick_sort_lomuto_partition in descending order")
    {
        cs::quick_sort_lomuto_partition<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("quick_sort_randomized_partition in ascending order")
    {
        cs::quick_sort_randomized_partition(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("quick_sort_randomized_partition in descending order")
    {
        cs::quick_sort_randomized_partition<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("selection_sort in ascending order")
    {
        cs::selection_sort(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("selection_sort in descending order")
    {
        cs::selection_sort<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("insertion_sort in ascending order")
    {
        cs::insertion_sort(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("insertion_sort in descending order")
    {
        cs::insertion_sort<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("bubble_sort in ascending order")
    {
        cs::bubble_sort(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("bubble_sort in descending order")
    {
        cs::bubble_sort<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }

    SECTION("heap_sort in ascending order")
    {
        cs::heap_sort(vec.begin(), vec.end());
        REQUIRE(vec == ascend_vec);
    }

    SECTION("heap_sort in descending order")
    {
        cs::heap_sort<std::vector<int>::iterator, ReverseComparator<int> >(vec.begin(), vec.end());
        REQUIRE(vec == descen_vec);
    }
}

TEST_CASE("BinarySearch", "[misc]")
{
    std::vector<int> sorted_vec{ -10, -6, 1, 2, 3, 5, 7, 8, 8, 15, 48 };

    for (int i = sorted_vec[0] - 2; i <= sorted_vec[0] + 2; i++)
    {
        std::pair<size_t, bool> search_result = cs::BinarySearch(sorted_vec, i);
        size_t lower_bound = static_cast<size_t>(std::lower_bound(sorted_vec.begin(), sorted_vec.end(), i) - sorted_vec.begin());
        auto find_result = std::find(sorted_vec.begin(), sorted_vec.end(), i);
        REQUIRE(search_result.first == lower_bound);
        REQUIRE((find_result != sorted_vec.end()) == search_result.second);
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

        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(1)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(2)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(0), /*to*/ g.GetVertexById(3)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(1), /*to*/ g.GetVertexById(2)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(1), /*to*/ g.GetVertexById(4)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(2), /*to*/ g.GetVertexById(5)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(2), /*to*/ g.GetVertexById(6)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(6), /*to*/ g.GetVertexById(7)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(7), /*to*/ g.GetVertexById(8)));
        REQUIRE(cs::FindDirectedEdge(/*from*/ g.GetVertexById(8), /*to*/ g.GetVertexById(9)));

        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(1)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(2)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(0), /*b*/ g.GetVertexById(3)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(1), /*b*/ g.GetVertexById(2)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(1), /*b*/ g.GetVertexById(4)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(2), /*b*/ g.GetVertexById(5)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(2), /*b*/ g.GetVertexById(6)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(6), /*b*/ g.GetVertexById(7)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(7), /*b*/ g.GetVertexById(8)));
        REQUIRE(cs::FindUndirectedEdge(/*a*/ g.GetVertexById(8), /*b*/ g.GetVertexById(9)));

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
        // In this particular graph every vertex is a strongly connected component
        // (which may not be true for every graph out there)
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

TEST_CASE("Graph input-output", "[graph]")
{
    using graph_type = cs::Graph<>;

    const char* path_adjacency_list = "./graph/adjacency_list.txt";
    const char* path_vertex_edge_list = "./graph/vertex_edge_list.txt";

    graph_type graph_adjacency_list;
    {
        std::ifstream ifs(path_adjacency_list);
        REQUIRE(ifs);

        cs::read_adjacency_list(ifs, graph_adjacency_list);
    }

    graph_type graph_vertex_edge_list;
    {
        std::ifstream ifs(path_vertex_edge_list);
        REQUIRE(ifs);

        cs::read_vertex_edge_list(ifs, graph_vertex_edge_list);
    }

    REQUIRE(graph_adjacency_list.VerticesNumber() == graph_vertex_edge_list.VerticesNumber());
    REQUIRE(graph_adjacency_list.EdgesNumber() == graph_vertex_edge_list.EdgesNumber());
}

TEST_CASE("Dijkstra's shortest path algorithm", "[graph]")
{
    using graph_type = cs::Graph<>;
    using vertex_type = graph_type::vertex_type;
    using id_type = graph_type::id_type;
    using length_type = graph_type::length_type;

    const char* graph_file_path = "./dijkstra/vertex_edge_list.txt";

    graph_type graph;
    {
        std::ifstream ifs(graph_file_path);
        REQUIRE(ifs);

        cs::read_vertex_edge_list(ifs, graph);
    }

    SECTION("directed graph")
    {
        std::vector<id_type> shortest_path_ethalon;
        {
            std::ifstream ifs("./dijkstra/shortest_path_directed_from_1_to_4.txt");
            REQUIRE(ifs);

            ifs >> shortest_path_ethalon;
        }

        std::vector<id_type> shortest_path;
        cs::DijkstraShortestPath_Directed<id_type, length_type>(
            /*graph*/ graph,
            /*from*/ 1,
            /*to*/ 4,
            /*visitShortestPath*/
            [&shortest_path](vertex_type& vertex, length_type /*shortestPathLength*/)
            {
                shortest_path.push_back(vertex.Id());
            });

        REQUIRE(shortest_path == shortest_path_ethalon);
    }

    SECTION("undirected graph")
    {
        std::vector<id_type> shortest_path_ethalon;
        {
            std::ifstream ifs("./dijkstra/shortest_path_undirected_from_1_to_4.txt");
            REQUIRE(ifs);

            ifs >> shortest_path_ethalon;
        }

        std::vector<id_type> shortest_path;
        cs::DijkstraShortestPath_Undirected<id_type, length_type>(
            /*graph*/ graph,
            /*from*/ 1,
            /*to*/ 4,
            /*visitShortestPath*/
            [&shortest_path](vertex_type& vertex, length_type /*shortestPathLength*/)
            {
                shortest_path.push_back(vertex.Id());
            });

        REQUIRE(shortest_path == shortest_path_ethalon);
    }
}

TEST_CASE("Heap", "[heap]")
{
    std::vector<int> vec{ 5, 2, 8, 15, 48, 1, -6, 7, 3, 8, -10 };
    std::vector<int> ascend_vec{ -10, -6, 1, 2, 3, 5, 7, 8, 8, 15, 48 };

    cs::MinHeap<int> heap;
    for (auto x : vec)
        heap.insert(x);

    for (auto x : ascend_vec)
    {
        REQUIRE(x == heap.top());
        heap.pop();
    }
}

TEST_CASE("BinaryTreeNode", "[binarytree]")
{
    using tree_node = cs::BinaryTreeNode<std::string, int>;

    // root
    // |--left1
    // |  |--left2
    // |  `--
    // `--right1
    //    |--
    //    `--right2

    tree_node* root = new tree_node(
        /*key*/ "root",
        /*left*/ new tree_node(
            /*key*/ "left1",
            /*left*/ new tree_node(
                /*key*/ "left2",
                /*left*/ nullptr,
                /*right*/ nullptr),
            /*right*/ nullptr
        ),
        /*right*/ new tree_node(
            /*key*/ "right1",
            /*left*/ nullptr,
            /*right*/ new tree_node(
                /*key*/ "right2",
                /*left*/ nullptr,
                /*right*/nullptr)
        )
    );

    REQUIRE(root->Parent() == nullptr);
    REQUIRE(root->Left()->Parent() == root);
    REQUIRE(root->Right()->Parent() == root);
    REQUIRE(root->Left()->Left()->Parent() == root->Left());
    REQUIRE(root->Right()->Right()->Parent() == root->Right());

    REQUIRE(root->Left()->Key() == "left1");
    REQUIRE(root->Right()->Key() == "right1");
    REQUIRE(root->Left()->Left()->Key() == "left2");
    REQUIRE(root->Right()->Right()->Key() == "right2");

    //cs::PrintTree<tree_node>(std::cout, root);

    cs::DeleteTree(root);
}

TEST_CASE("BinarySearchTree", "[binarytree]")
{
    using bst_type = cs::BinarySearchTree<int, std::string>;

    bst_type bst;

    bst.insert(4, "four");
    bst.insert(2, "two");
    bst.insert(6, "six");
    bst.insert(1, "one");
    bst.insert(3, "three");
    bst.insert(5, "five");
    bst.insert(7, "seven");

    //cs::PrintTree<typename bst_type::tree_node>(std::cout, bst.Root());
    /*
               4
              / \
             /   \
            2     6
           / \   / \
          1   3 5   7
    */

    SECTION("tree structure")
    {
        REQUIRE(bst.Root()->Key() == 4);
        REQUIRE(bst.Root()->Left()->Key() == 2);
        REQUIRE(bst.Root()->Right()->Key() == 6);
        REQUIRE(bst.Root()->Left()->Left()->Key() == 1);
        REQUIRE(bst.Root()->Left()->Right()->Key() == 3);
        REQUIRE(bst.Root()->Right()->Left()->Key() == 5);
        REQUIRE(bst.Root()->Right()->Right()->Key() == 7);

        REQUIRE(bst.Root()->Parent() == nullptr);
        REQUIRE(bst.Root()->Left()->Parent() == bst.Root());
        REQUIRE(bst.Root()->Right()->Parent() == bst.Root());
        REQUIRE(bst.Root()->Left()->Left()->Parent() == bst.Root()->Left());
        REQUIRE(bst.Root()->Left()->Right()->Parent() == bst.Root()->Left());
        REQUIRE(bst.Root()->Right()->Left()->Parent() == bst.Root()->Right());
        REQUIRE(bst.Root()->Right()->Right()->Parent() == bst.Root()->Right());

        REQUIRE(cs::Height(bst.find(1)) == 0);
        REQUIRE(cs::Height(bst.find(3)) == 0);
        REQUIRE(cs::Height(bst.find(5)) == 0);
        REQUIRE(cs::Height(bst.find(7)) == 0);
        REQUIRE(cs::Height(bst.find(2)) == 1);
        REQUIRE(cs::Height(bst.find(6)) == 1);
        REQUIRE(cs::Height(bst.find(4)) == 2);
    }

    SECTION("change values")
    {
        REQUIRE(bst.at(1) == "one");
        REQUIRE(bst.at(2) == "two");
        REQUIRE(bst.at(3) == "three");
        REQUIRE(bst.at(4) == "four");
        REQUIRE(bst.at(5) == "five");
        REQUIRE(bst.at(6) == "six");
        REQUIRE(bst.at(7) == "seven");

        bst[1] = "1st";
        REQUIRE(bst[1] == "1st");

        bst[2] = "2nd";
        REQUIRE(bst[2] == "2nd");

        bst[3] = "3rd";
        REQUIRE(bst[3] == "3rd");

        bst[4] = "4th";
        REQUIRE(bst[4] == "4th");

        bst[5] = "5th";
        REQUIRE(bst[5] == "5th");

        bst[6] = "6th";
        REQUIRE(bst[6] == "6th");

        bst[7] = "7th";
        REQUIRE(bst[7] == "7th");

        bst[0] = "zero";
        REQUIRE(bst[0] == "zero");

        bst[8] = "8th";
        REQUIRE(bst[8] == "8th");
    }

    SECTION("remove root")
    {
        bst.remove(4);

        REQUIRE(bst.Root()->Key() == 3);
        REQUIRE(bst.Root()->Left()->Key() == 2);
        REQUIRE(bst.Root()->Right()->Key() == 6);
        REQUIRE(bst.Root()->Left()->Left()->Key() == 1);
        REQUIRE(bst.Root()->Left()->Right() == nullptr);
        REQUIRE(bst.Root()->Right()->Left()->Key() == 5);
        REQUIRE(bst.Root()->Right()->Right()->Key() == 7);
    }

    SECTION("remove inner node")
    {
        bst.remove(6);

        REQUIRE(bst.Root()->Key() == 4);
        REQUIRE(bst.Root()->Left()->Key() == 2);
        REQUIRE(bst.Root()->Right()->Key() == 5);
        REQUIRE(bst.Root()->Left()->Left()->Key() == 1);
        REQUIRE(bst.Root()->Left()->Right()->Key() == 3);
        REQUIRE(bst.Root()->Right()->Left() == nullptr);
        REQUIRE(bst.Root()->Right()->Right()->Key() == 7);
    }

    SECTION("remove leaf")
    {
        bst.remove(3);

        REQUIRE(bst.Root()->Key() == 4);
        REQUIRE(bst.Root()->Left()->Key() == 2);
        REQUIRE(bst.Root()->Right()->Key() == 6);
        REQUIRE(bst.Root()->Left()->Left()->Key() == 1);
        REQUIRE(bst.Root()->Left()->Right() == nullptr);
        REQUIRE(bst.Root()->Right()->Left()->Key() == 5);
        REQUIRE(bst.Root()->Right()->Right()->Key() == 7);
    }
}

TEST_CASE("AvlTree", "[binarytree]")
{
    using bst_type = cs::AvlTree<int, std::string>;

    bst_type bst;

    bst.insert(1, "one");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(2, "two");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(3, "three");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(4, "four");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(5, "five");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(6, "six");
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.insert(7, "seven");
    REQUIRE(cs::IsBalanced(bst.Root()));

    //cs::PrintTree<typename bst_type::tree_node>(std::cout, bst.Root());

    bst.remove(4);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(1);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(6);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(5);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(2);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(7);
    REQUIRE(cs::IsBalanced(bst.Root()));

    bst.remove(3);
    REQUIRE(cs::IsBalanced(bst.Root()));
}

TEST_CASE("BTree", "[btree]")
{
    using btree_type = cs::BTree<5, int, std::string>;

    btree_type btree;

    btree.insert(1, "one");
    btree.insert(8, "eight");
    btree.insert(2, "two");
    btree.insert(0, "zero");
    btree.insert(5, "five");
    btree.insert(6, "six");
    btree.insert(3, "three");
    btree.insert(4, "four");
    btree.insert(9, "nine");
    btree.insert(7, "seven");
    btree.insert(10, "ten");
    btree.insert(11, "eleven");
    btree.insert(12, "twelve");
    btree.insert(13, "thirteen");

    //btree.print(std::cout);

    SECTION("change values")
    {
        REQUIRE(btree.at(0) == "zero");
        REQUIRE(btree.at(1) == "one");
        REQUIRE(btree.at(2) == "two");
        REQUIRE(btree.at(3) == "three");
        REQUIRE(btree.at(4) == "four");
        REQUIRE(btree.at(5) == "five");
        REQUIRE(btree.at(6) == "six");
        REQUIRE(btree.at(7) == "seven");
        REQUIRE(btree.at(8) == "eight");
        REQUIRE(btree.at(9) == "nine");
        REQUIRE(btree.at(10) == "ten");
        REQUIRE(btree.at(11) == "eleven");
        REQUIRE(btree.at(12) == "twelve");
        REQUIRE(btree.at(13) == "thirteen");

        btree[1] = "1st";
        REQUIRE(btree[1] == "1st");

        btree[2] = "2nd";
        REQUIRE(btree[2] == "2nd");

        btree[3] = "3rd";
        REQUIRE(btree[3] == "3rd");

        btree[4] = "4th";
        REQUIRE(btree[4] == "4th");

        btree[5] = "5th";
        REQUIRE(btree[5] == "5th");

        btree[6] = "6th";
        REQUIRE(btree[6] == "6th");

        btree[7] = "7th";
        REQUIRE(btree[7] == "7th");

        btree[0] = "zero";
        REQUIRE(btree[0] == "zero");

        btree[8] = "8th";
        REQUIRE(btree[8] == "8th");
    }

    SECTION("remove items")
    {
        for (int i = 0; i <= 13; i++)
        {
            btree_type::iterator it = btree.find(i);
            REQUIRE(it);
            REQUIRE(it.key() == i);
            btree.remove(i);
            REQUIRE(!btree.find(i));
        }
    }
}

TEST_CASE("HashMap", "[hash]")
{
    using key_type = int;
    using value_type = std::string;

    cs::HashMap<key_type, value_type> hashMap;
    REQUIRE(hashMap.size() == 0);

    //auto printInfo = [&hashMap]()
    //{
    //    std::cout << "elements = " << hashMap.size() << " buckets = " << hashMap.buckets_number() << " load = " << hashMap.load() << std::endl;
    //    hashMap.visit_bucket_sizes(
    //        [](size_t size)
    //        {
    //            std::cout << size << ' ';
    //        });
    //    std::cout << std::endl;

    //    hashMap.visit(
    //        [](const key_type& key, const value_type& value)
    //        {
    //            std::cout << key << " --> " << value << ' ';
    //        }
    //    );
    //    std::cout << std::endl;
    //    std::cout << std::endl;
    //};

    hashMap.insert(1, "one");
    REQUIRE(hashMap.size() == 1);
    REQUIRE(hashMap.at(1) == "one");
    REQUIRE(hashMap[1] == "one");
    //printInfo();

    hashMap[2] = "two";
    REQUIRE(hashMap.size() == 2);
    REQUIRE(hashMap.at(2) == "two");
    REQUIRE(hashMap[2] == "two");
    //printInfo();

    hashMap.insert(3, "three");
    REQUIRE(hashMap.size() == 3);
    REQUIRE(hashMap.at(3) == "three");
    REQUIRE(hashMap[3] == "three");
    //printInfo();

    hashMap[4] = "four";
    REQUIRE(hashMap.size() == 4);
    REQUIRE(hashMap.at(4) == "four");
    REQUIRE(hashMap[4] == "four");
    //printInfo();

    hashMap.insert(5, "five");
    REQUIRE(hashMap.size() == 5);
    REQUIRE(hashMap.at(5) == "five");
    REQUIRE(hashMap[5] == "five");
    //printInfo();

    hashMap[6] = "six";
    REQUIRE(hashMap.size() == 6);
    REQUIRE(hashMap.at(6) == "six");
    REQUIRE(hashMap[6] == "six");
    //printInfo();

    hashMap.insert(7, "seven");
    REQUIRE(hashMap.size() == 7);
    REQUIRE(hashMap.at(7) == "seven");
    REQUIRE(hashMap[7] == "seven");
    //printInfo();

    hashMap[8] = "eight";
    REQUIRE(hashMap.size() == 8);
    REQUIRE(hashMap.at(8) == "eight");
    REQUIRE(hashMap[8] == "eight");
    //printInfo();

    hashMap.insert(9, "nine");
    REQUIRE(hashMap.size() == 9);
    REQUIRE(hashMap.at(9) == "nine");
    REQUIRE(hashMap[9] == "nine");
    //printInfo();

    // --- erase ---

    hashMap.erase(9);
    REQUIRE(hashMap.size() == 8);
    //printInfo();

    hashMap.erase(8);
    REQUIRE(hashMap.size() == 7);
    //printInfo();

    hashMap.erase(7);
    REQUIRE(hashMap.size() == 6);
    //printInfo();

    hashMap.erase(6);
    REQUIRE(hashMap.size() == 5);
    //printInfo();

    hashMap.erase(5);
    REQUIRE(hashMap.size() == 4);
    //printInfo();

    hashMap.erase(4);
    REQUIRE(hashMap.size() == 3);
    //printInfo();

    hashMap.erase(3);
    REQUIRE(hashMap.size() == 2);
    //printInfo();

    hashMap.erase(2);
    REQUIRE(hashMap.size() == 1);
    //printInfo();

    hashMap.erase(1);
    REQUIRE(hashMap.size() == 0);
    //printInfo();
}

TEST_CASE("BloomFilter", "[bloom]")
{
    const unsigned int NBits = 59;
    const unsigned int NHashes = 3;

    cs::BloomFilter<std::string, NBits, NHashes> bloomFilter;

    bloomFilter.insert("zero");
    bloomFilter.insert("one");
    bloomFilter.insert("two");
    bloomFilter.insert("three");
    bloomFilter.insert("four");
    bloomFilter.insert("five");
    bloomFilter.insert("six");
    bloomFilter.insert("seven");
    bloomFilter.insert("eight");
    bloomFilter.insert("nine");

    //bloomFilter.print(std::cout); std::cout << std::endl;

    REQUIRE(bloomFilter.contains("zero"));
    REQUIRE(bloomFilter.contains("one"));
    REQUIRE(bloomFilter.contains("two"));
    REQUIRE(bloomFilter.contains("three"));
    REQUIRE(bloomFilter.contains("four"));
    REQUIRE(bloomFilter.contains("five"));
    REQUIRE(bloomFilter.contains("six"));
    REQUIRE(bloomFilter.contains("seven"));
    REQUIRE(bloomFilter.contains("eight"));
    REQUIRE(bloomFilter.contains("nine"));

    size_t false_positives = 0;
    if (bloomFilter.contains("ten")) ++false_positives;
    if (bloomFilter.contains("eleven")) ++false_positives;
    if (bloomFilter.contains("twelve")) ++false_positives;
    if (bloomFilter.contains("thirteen")) ++false_positives;
    if (bloomFilter.contains("fourteen")) ++false_positives;
    if (bloomFilter.contains("fifteen")) ++false_positives;
    if (bloomFilter.contains("sixteen")) ++false_positives;
    if (bloomFilter.contains("seventeen")) ++false_positives;
    if (bloomFilter.contains("eighteen")) ++false_positives;
    if (bloomFilter.contains("nineteen")) ++false_positives;

    REQUIRE(false_positives <= 2);
}

TEST_CASE("MinimalSpanningTree", "[mst]")
{
    using graph_type = cs::Graph<>;
    using edge_type = typename graph_type::edge_type;
    using id_type = typename graph_type::id_type;
    using length_type = typename graph_type::length_type;

    SECTION("triangle")
    {
        /*
           1       0 --> 1 (len=1)
          / \      1 --> 2 (len=2)
         0 - 2     0 --> 2 (len=3)
        */

        const char* path_vertex_edge_list = "./graph/triangle.txt";

        graph_type graph;
        {
            std::ifstream ifs(path_vertex_edge_list);
            REQUIRE(ifs);
            cs::read_vertex_edge_list(ifs, graph);
        }

        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Prim<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 3);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Kruskal<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 3);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
    }

    SECTION("rectangle")
    {
        /*
         1---2     0 --> 1 (len=1)
         | \ |     1 --> 2 (len=1)
         0---3     1 --> 3 (len=1)
                   2 --> 3 (len=2)
                   0 --> 3 (len=2)
        */

        const char* path_vertex_edge_list = "./graph/rectangle.txt";

        graph_type graph;
        {
            std::ifstream ifs(path_vertex_edge_list);
            REQUIRE(ifs);
            cs::read_vertex_edge_list(ifs, graph);
        }

        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Prim<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 3);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Kruskal<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 3);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
    }

    SECTION("pentagon")
    {
        /*
           0---1     0 --> 1 (len=2)
          /|   |\    0 --> 2 (len=1)
         2-+---+-3   0 --> 3 (len=3)
          \ \ / /    0 --> 4 (len=1)
           \ | /     1 --> 2 (len=2)
            \|/      1 --> 3 (len=1)
             4       1 --> 4 (len=1)
                     2 --> 3 (len=3)
                     2 --> 4 (len=2)
                     3 --> 4 (len=2)
        */

        const char* path_vertex_edge_list = "./graph/pentagon.txt";

        graph_type graph;
        {
            std::ifstream ifs(path_vertex_edge_list);
            REQUIRE(ifs);
            cs::read_vertex_edge_list(ifs, graph);
        }

        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Prim<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 4);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
        {
            length_type len = 0;
            size_t n_edges = 0;
            cs::VisitMinimumSpanningTree_Kruskal<id_type, length_type>(
                graph,
                [&len, &n_edges](edge_type& edge) -> void
                {
                    len += edge.Length();
                    ++n_edges;
                    //std::cout << edge.From().Id() << " --> " << edge.To().Id() << " (" << edge.Length() << ')' << std::endl;
                });
            REQUIRE(len == 4);
            REQUIRE(n_edges == graph.VerticesNumber() - 1);
        }
        //std::cout << "--------------------" << std::endl;
    }
}

TEST_CASE("HuffmanEncoding", "[huffman]")
{
    using TSymbol = char;
    using TFrequency = int;

    std::vector<std::pair<TSymbol, TFrequency> > alphabet;
    alphabet.reserve(10);

    // Frequencies were selected to be power of 2 so that codes of all symbols were unambiguous
    alphabet.emplace_back('a', 1024);
    alphabet.emplace_back('b', 512);
    alphabet.emplace_back('c', 256);
    alphabet.emplace_back('d', 128);
    alphabet.emplace_back('e', 64);
    alphabet.emplace_back('f', 32);
    alphabet.emplace_back('g', 16);
    alphabet.emplace_back('h', 8);
    alphabet.emplace_back('i', 4);
    alphabet.emplace_back('j', 2);

    cs::HuffmanEncoding<TSymbol, TFrequency> encoding(alphabet);

    for (size_t i = 0; i < alphabet.size(); ++i)
    {
        TSymbol symbol = alphabet[i].first;
        std::vector<bool> code = encoding.Encode(symbol);
        TSymbol decoded = encoding.Decode(code);

        //TFrequency frequency = alphabet[i].second;
        //std::cout << "symbol    = " << symbol << std::endl;
        //std::cout << "decoded   = " << decoded << std::endl;
        //std::cout << "frequency = " << frequency << std::endl;
        //std::cout << "code      = ";
        //print(std::cout, code.cbegin(), code.cend(), "");
        //std::cout << std::endl << std::endl;

        REQUIRE(symbol == decoded);

        if (i < alphabet.size() - 1)
        {
            REQUIRE(code.size() == i + 1);
        }
        else
        {
            // last symbol code has the same length as penultimate symbol
            REQUIRE(code.size() == i);
        }
    }
}

TEST_CASE("PackKnapsack", "[knapsack]")
{
    using TValue = int;
    auto plus = [](const TValue& a, const cs::KnapsackItem<TValue>& b) -> TValue { return a + b.value; };

    SECTION("case1")
    {
        std::vector<cs::KnapsackItem<TValue> > items
        {
            cs::KnapsackItem<TValue>(/*weight*/ 4, /*value*/ 3),
            cs::KnapsackItem<TValue>(/*weight*/ 3, /*value*/ 2),
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 4), // *
            cs::KnapsackItem<TValue>(/*weight*/ 3, /*value*/ 4)  // *
        };

        std::vector<cs::KnapsackItem<TValue> > knapsack = cs::PackKnapsack<TValue>(/*maxWeight*/ 6, /*items*/ items);
        REQUIRE(std::accumulate(knapsack.begin(), knapsack.end(), TValue(0), plus) == 8);
    }

    SECTION("case2")
    {
        std::vector<cs::KnapsackItem<TValue> > items
        {
            cs::KnapsackItem<TValue>(/*weight*/ 9, /*value*/ 9),
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2), // *
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2), // *
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2), // *
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 4)  // *
        };

        std::vector<cs::KnapsackItem<TValue> > knapsack = cs::PackKnapsack<TValue>(/*maxWeight*/ 10, /*items*/ items);
        REQUIRE(std::accumulate(knapsack.begin(), knapsack.end(), TValue(0), plus) == 10);
    }

    SECTION("case3")
    {
        std::vector<cs::KnapsackItem<TValue> > items
        {
            cs::KnapsackItem<TValue>(/*weight*/ 9, /*value*/ 11), // *
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2),
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2),
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 2),
            cs::KnapsackItem<TValue>(/*weight*/ 2, /*value*/ 4)
        };

        std::vector<cs::KnapsackItem<TValue> > knapsack = cs::PackKnapsack<TValue>(/*maxWeight*/ 10, /*items*/ items);
        REQUIRE(std::accumulate(knapsack.begin(), knapsack.end(), TValue(0), plus) == 11);
    }
}