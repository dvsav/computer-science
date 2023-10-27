#include "print.h"      // for cs::print
#include "merge_sort.h" // for cs::merge_sort
#include "quick_sort.h" // for cs::quick_sort_lomuto_partition, cs::quick_sort_randomized_partition
#include "utility.h"    // for cs::ReverseComparator, cs::files_textually_equal

#include <array>        // for std::array
#include <vector>       // for std::vector

// Catch2 - a single header unit test framework
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "Catch2/catch.hpp"

unsigned int Factorial(unsigned int number) {
    return number <= 1 ? number : Factorial(number - 1) * number;
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