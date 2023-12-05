#pragma once

#include "utility.h"  // for DefaultComparator

#include <functional> // for std::function
#include <vector>     // for std::vector
#include <utility>    // for std::pair

namespace cs
{
    template <typename T, typename K, typename TComparator = DefaultComparator<K> >
    std::pair<size_t, bool> BinarySearch(
        const std::vector<T>& sortedArray,
        const K& target,
        std::function<K(const T&)> extractKey)
    {
        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
            bool (*)(const K&, const K&)>::value,
            "LessThan function is missing in TComparator");

        // Check for the existence of a member function "EqualTo" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::EqualTo),
            bool (*)(const K&, const K&)>::value,
            "EqualTo function is missing in TComparator");

        if (sortedArray.size() == 0)
            return std::make_pair(0, false);

        size_t left = 0;
        size_t right = sortedArray.size() - 1;

        if (TComparator::LessThan(target, extractKey(sortedArray[left])))
            return std::make_pair(left, false);

        if (TComparator::LessThan(extractKey(sortedArray[right]), target))
            return std::make_pair(right + 1, false);

        while (true)
        {
            size_t mid = left + (right - left) / 2;

            // Check if the target is at the middle
            if (TComparator::EqualTo(extractKey(sortedArray[mid]), target))
            {
                return std::make_pair(mid, true); // Found the target
            }

            if (left == right)
            {
                return std::make_pair(
                    TComparator::LessThan(target, extractKey(sortedArray[left])) ? left : left + 1,
                    false);
            }

            // If target is greater, ignore left half
            if (TComparator::LessThan(extractKey(sortedArray[mid]), target))
            {
                left = std::min(mid + 1, right);
            }
            else
            {
                // If target is smaller, ignore right half
                right = std::max(mid - 1, left);
            }
        }
    }

    template <typename T, typename TComparator = DefaultComparator<T> >
    std::pair<size_t, bool> BinarySearch(
        const std::vector<T>& sortedArray,
        const T& target)
    {
        return BinarySearch<T, T, TComparator>(
            sortedArray,
            target,
            [](const T& x) { return x; });
    }
} // namespace cs
