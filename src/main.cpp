#include "print.h"      // for cs::print
#include "merge_sort.h" // for cs::merge_sort
#include "quick_sort.h" // for cs::quick_sort_lomuto_partition, cs::quick_sort_randomized_partition
#include "utility.h"    // for cs::ReverseComparator

#include <array>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<int> vec{ 5, 2, 8, 15, 48, 1, -6, 7, 3, 8, -10 };

    cs::print(vec.begin(), vec.end(), " ");
    std::cout << std::endl;

    //cs::merge_sort<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    //cs::merge_sort(vec.begin(), vec.end());
    //cs::quick_sort_lomuto_partition(vec.begin(), vec.end());
    //cs::quick_sort_lomuto_partition<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    //cs::quick_sort_randomized_partition(vec.begin(), vec.end());
    cs::quick_sort_randomized_partition<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    
    cs::print(vec.begin(), vec.end(), " ");
    std::cout << std::endl;

    return 0;
}