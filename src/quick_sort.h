#pragma once

#include "utility.h"

#include <algorithm>

namespace cs
{
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    TIterator lomuto_partition(TIterator begin, TIterator end)
    {
        // pick the last element in the range as the pivot element
        auto pivot = end - 1;
        auto i = begin;

        for (auto j = begin; j != end; j++)
        {
            if (TComparator::LessThanOrEqualTo(*j, *pivot))
            {
                std::swap(*i, *j);
                i++;
            }
        }

        // Everything in range [0...i-2] is <= pivot
        // Everything in range [i...end) is > pivot
        return i - 1;
    }

    /**
     * @brief Sort specified range of values using quick sort algorithm using Lomuto partitioning scheme.
     *
     * @tparam TIterator Iterator type (must be a random access iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThanOrEqualTo(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void quick_sort_lomuto_partition(TIterator begin, TIterator end)
    {
        // Check that TIterator is a random-access iterator
        static_assert(
            std::is_same<typename std::iterator_traits<TIterator>::iterator_category,
            std::random_access_iterator_tag>::value,
            "TIterator must be a random-access iterator.");

        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThanOrEqualTo),
            bool (*)(const TIterator::value_type&, const TIterator::value_type&)>::value,
            "LessThanOrEqualTo function is missing in TComparator");

        auto size = end - begin;

        if (size > 1)
        {
            auto pivot = lomuto_partition<TIterator, TComparator>(begin, end);
            quick_sort_lomuto_partition<TIterator, TComparator>(begin, pivot);
            quick_sort_lomuto_partition<TIterator, TComparator>(pivot + 1, end);
        }
    }
}
