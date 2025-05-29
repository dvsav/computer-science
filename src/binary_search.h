#pragma once

#include "utility.h"  // for DefaultComparator

#include <functional> // for std::function
#include <vector>     // for std::vector
#include <utility>    // for std::pair

namespace cs
{
    /**
     * @brief Performs binary search (dikhotomy) of an element @p target
     * in a sorted array @p sortedArray.
     * Complexity: O(lgN) where N is the number of elements in the sorted array.
     * 
     * @param sortedArray - the sorted array containing element of type T.
     * @param target - the element (key) of type K searched for in the sorted array.
     * @param extractKey - a function that takes an element from the sorted array
     * and returns a key that is compared to the @p target.
     * 
     * @return a pair (integer, bool) in which the second is a boolean flag which is true
     * if the searched @p target is found and false otherwise. The first is an integer index
     * of the found element if it was found and if it's not found - the index at which
     * the @p target would be inserted in the sorted array.
     * 
     * @tparam T type of an elemennt in the @p sortedArray.
     * @tparam K type of the @p target (key) which is searched for in the @p sortedArray.
     * @tparam TComparator comparator used to compare the keys (must have a static member
     * function bool LessThan(const K& a, const K& b) where K is the key type.
     * For example @see DefaultComparator, @see ReverseComparator.
     */
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
            // Divide the search range in half
            size_t mid = left + (right - left) / 2;

            // Check if the target is at the middle
            if (TComparator::EqualTo(extractKey(sortedArray[mid]), target))
            {
                return std::make_pair(mid, true); // Found the target
            }

            // If the serch range has shrinked to 1 element
            if (left == right)
            {
                return std::make_pair(
                    TComparator::LessThan(target, extractKey(sortedArray[left])) ? left : left + 1,
                    false);
            }

            if (TComparator::LessThan(extractKey(sortedArray[mid]), target))
            {
                // If target is greater, ignore left half
                left = std::min(mid + 1, right);
            }
            else
            {
                // If target is smaller, ignore right half
                right = std::max(mid - 1, left);
            }
        }
    }

    /**
     * @brief Performs binary search (dikhotomy) of an element @p target
     * in a sorted array @p sortedArray.
     * Complexity: O(lgN) where N is the number of elements in the sorted array.
     *
     * @param sortedArray - the sorted array containing element of type T.
     * @param target - the element of type T searched for in the sorted array.
     *
     * @return a pair (integer, bool) in which the second is a boolean flag which is true
     * if the searched @p target is found and false otherwise. The first is an integer index
     * of the found element if it was found and if it's not found - the index at which
     * the @p target would be inserted in the sorted array.
     *
     * @tparam T type of an elemennt in the @p sortedArray.
     * @tparam TComparator comparator used to compare the elements (must have a static member
     * function bool LessThan(const T& a, const T& b) where T is the element type.
     * For example @see DefaultComparator, @see ReverseComparator.
     */
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
