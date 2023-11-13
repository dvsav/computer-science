#pragma once

#include <algorithm> // for std::swap, std::min_element
#include <iterator>  // for std::next, std::prev

namespace cs {
    /**
     * @brief Sort specified range of values using selection sort algorithm.
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

        for (auto it = std::next(begin); it != end; ++it) 
        {
            auto key = *it;
            auto pos = it;

            while (pos != begin && TComparator::LessThan(key, *std::prev(pos))) 
            {
                *pos = *std::prev(pos);
                pos = std::prev(pos);
            }
            *pos = key;
        }
    }

    /**
     * @brief Sort specified range of values using bubble sort algorithm.
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

        for (auto sorted_part = end; sorted_part != begin; --sorted_part)
        {
            for (auto it = begin; it != std::prev(sorted_part); ++it)
            {
                if (TComparator::LessThan(*std::next(it), *it))
                {
                    std::swap(*std::next(it), *it);
                }
            }
        }
    }
}
