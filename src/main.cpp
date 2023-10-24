#include "print.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "utility.h"

#include <algorithm>
#include <array>
#include <list>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<int> vec{ 5, 2, 8, 15, 48, 1, -6, 7 };
    //cs::merge_sort<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    //cs::merge_sort(vec.begin(), vec.end());
    //cs::quick_sort(vec.begin(), vec.end());
    cs::quick_sort_lomuto_partition<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    cs::print(vec.begin(), vec.end(), " ");

    return 0;
}