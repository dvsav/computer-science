#pragma once

#include "utility.h"

#include <algorithm> // for std::copy
#include <iterator>  // for std::iterator_traits
#include <vector>    // for std::vector

namespace cs
{
    template <typename T, typename TComparator = DefaultComparator<T> >
    static std::vector<T> merge(const std::vector<T>& a, const std::vector<T>& b)
    {
        std::vector<T> result;
        result.reserve(a.size() + b.size());

        size_t a_i = 0;
        size_t b_i = 0;

        while (a_i < a.size() && b_i < b.size())
        {
            if (TComparator::LessThan(a[a_i], b[b_i]))
                result.push_back(a[a_i++]);
            else
                result.push_back(b[b_i++]);
        }

        while (a_i < a.size())
            result.push_back(a[a_i++]);

        while (b_i < b.size())
            result.push_back(b[b_i++]);

        return result;
    }

    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    static std::vector<typename TIterator::value_type> merge_sort_internal(TIterator begin, TIterator end)
    {
        auto size = end - begin;

        if (size == 0)
            return std::vector<typename TIterator::value_type>();
        else if (size == 1)
            return std::vector<typename TIterator::value_type>(1, *begin);

        auto middle = begin + size / 2;

        auto left_part = merge_sort_internal<TIterator, TComparator>(begin, middle);
        auto right_part = merge_sort_internal<TIterator, TComparator>(middle, end);

        return merge<typename TIterator::value_type, TComparator>(left_part, right_part);
    }

    /**
     * @brief Sort specified range of values using merge sort algorithm.
     *
     * @tparam TIterator Iterator type (must be a random access iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void merge_sort(TIterator begin, TIterator end)
    {
        // Check that TIterator is a random-access iterator
        static_assert(
            std::is_same<typename std::iterator_traits<TIterator>::iterator_category,
                         std::random_access_iterator_tag>::value,
            "TIterator must be a random-access iterator.");

        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
                         bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThan function is missing in TComparator");

        auto result = merge_sort_internal<TIterator, TComparator>(begin, end);
        std::copy(result.begin(), result.end(), begin);
    }
}
