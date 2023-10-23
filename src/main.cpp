#include "print.h"
#include "merge_sort.h"

#include <algorithm>
#include <array>
#include <list>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<int> vec{ 5, 2, 8, 15, 1, -6, 7 };
    cs::merge_sort<std::vector<int>::iterator, cs::ReverseComparator<int> >(vec.begin(), vec.end());
    //cs::merge_sort(vec.begin(), vec.end());
    cs::print(vec.begin(), vec.end(), " ");

    return 0;
}