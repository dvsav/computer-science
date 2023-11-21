#pragma once

#include <algorithm> // for std::swap, std::min_element
#include <iterator>  // for std::next, std::prev

namespace cs
{
    /**
     * @brief Sort specified range of values using selection sort algorithm.
     * Idea: Find the minimal value and put it at the beginning of collection (N elements).
     * Then do the same for the rest of collection (N - 1 elements).
     * Runtime: O(N^2).
     *
     * @tparam TIterator Iterator type (enough to be a forward iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void selection_sort(TIterator begin, TIterator end) 
    {
        // Check that TIterator is compatible with a forward iterator
        static_assert(
            std::is_base_of<std::forward_iterator_tag,
                            typename std::iterator_traits<TIterator>::iterator_category>::value,
            "TIterator must be compatible with a forward iterator.");

        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
                         bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThan function is missing in TComparator");

        for (auto it = begin; it != end; ++it) 
        {
            auto min_it = std::min_element(it, end, TComparator::LessThan);
            if (it != min_it) 
                std::swap(*it, *min_it);
        }
    }

    /**
     * @brief Sort specified range of values using insertion sort algorithm.
     * Idea: Divide the collection into two parts: sorted array on the left
     * and unsorted array on the right. Sorted part is initially contains just one element.
     * Pick the 1st element from the unsorted part, insert it to the right place in the sorted part.
     * Repeat until the unsorted part becomes empty.
     * Runtime: best case (already sorted array) - O(N); worst case (reversely sorted array) - O(N^2).
     *
     * @tparam TIterator Iterator type (must be compatible with a bidirectional iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void insertion_sort(TIterator begin, TIterator end) 
    {
        // Check that TIterator is compatible with a bidirectional iterator
        static_assert(
            std::is_base_of<std::bidirectional_iterator_tag,
                            typename std::iterator_traits<TIterator>::iterator_category>::value,
            "TIterator must be compatible with a bidirectional iterator.");

        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
                         bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThan function is missing in TComparator");

        if (begin == end)
            return;

        // 'it' is the 'end' of the sorted part and the 'begin' of the unsorted part
        for (auto unsorted_begin = std::next(begin); unsorted_begin != end; ++unsorted_begin)
        {
            auto key = *unsorted_begin; // value of the 1st element in the unsorted part
            auto pos = unsorted_begin;  // position of an element in the sorted part
            // position of the element is shofted to the left until the element gets to its legal place
            for (; pos != begin && TComparator::LessThan(key, *std::prev(pos)); pos = std::prev(pos))
                *pos = *std::prev(pos);
            *pos = key;
        }
    }

    /**
     * @brief Sort specified range of values using bubble sort algorithm.
     * Idea: Iterate from the beginning to the end of collection swapping adjacent elements if they are in the wrong order.
     * As a result max element bubbles up to the end of collection. Start over from the beginning until no swaps happen.
     * Runtime: best case (already sorted array) - O(N); worst case (reversely sorted array) - O(N^2).
     *
     * @tparam TIterator Iterator type (must be compatible with a bidirectional iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void bubble_sort(TIterator begin, TIterator end) 
    {
        // Check that TIterator is compatible with a bidirectional iterator
        static_assert(
            std::is_base_of<std::bidirectional_iterator_tag,
                            typename std::iterator_traits<TIterator>::iterator_category>::value,
            "TIterator must be compatible with a bidirectional iterator.");

        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
                         bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThan function is missing in TComparator");

        for (auto sorted_part_begin = end; sorted_part_begin != begin; --sorted_part_begin)
        {
            bool sorted = true;
            for (auto it = begin; it != std::prev(sorted_part_begin); ++it)
            {
                if (TComparator::LessThan(*std::next(it), *it))
                {
                    std::swap(*std::next(it), *it);
                    sorted = false;
                }
            }
            // quit if no swaps happened
            if (sorted)
                break;
        }
    }
} // namespace cs
