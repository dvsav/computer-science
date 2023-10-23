#include "print.h"
#include "merge_sort.h"

#include <algorithm>
#include <array>
#include <list>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<int> vec{ 5, 2, 8, 15, 1, -6, 7 };
    sc::merge_sort(vec.begin(), vec.end());
    sc::print(vec.begin(), vec.end(), " ");

    return 0;
}