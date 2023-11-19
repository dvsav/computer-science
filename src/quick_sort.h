#pragma once

#include "utility.h" // for DefaultComparator

#include <algorithm> // for std::swap
#include <iterator>  // for std::iterator_traits
#include <random>    // for std::random_device, std::mt19937, std::uniform_int_distribution

namespace cs
{
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    TIterator partition(TIterator begin, TIterator end, TIterator pivot)
    {
        // Remember pivot element value
        auto pivot_value = *pivot;
        // Move pivot element to the end of the range. Why? -Because it should be swapped last to appear at the border of two partitions.
        // Why is it important that the pivot element end up at the border of two partitions? -Because then it can be excluded from further consideration, because its position will never change.
        std::swap(*pivot, *std::prev(end));

        auto i = begin;
        for (auto j = begin; j != end; j++)
        {
            if (TComparator::LessThanOrEqualTo(*j, pivot_value))
            {
                std::swap(*i, *j);
                i++;
            }
        }

        // Everything in range [0...i-2] is <= pivot
        // Everything in range [i...end) is > pivot
        return std::prev(i);
    }

    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    TIterator lomuto_partition(TIterator begin, TIterator end)
    {
        // Pick the last element in the range as the pivot element. Why? -Because it should be swapped last to appear at the border of two partitions.
        // Why is it important that the pivot element end up at the border of two partitions? -Because then it can be excluded from further consideration, because its position will never change.
        auto pivot = std::prev(end);

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
        // pivot element itself appears at position (i-1)
        // Everything in range [i...end) is > pivot
        return std::prev(i);
    }

    /**
     * @brief Sort specified range of values using quick sort algorithm with Lomuto partitioning scheme.
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

        // Check for the existence of a member function "LessThanOrEqualTo" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThanOrEqualTo),
            bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThanOrEqualTo function is missing in TComparator");

        auto size = end - begin;

        if (size > 1)
        {
            auto pivot = lomuto_partition<TIterator, TComparator>(begin, end);
            quick_sort_lomuto_partition<TIterator, TComparator>(begin, pivot);
            quick_sort_lomuto_partition<TIterator, TComparator>(std::next(pivot), end);
        }
    }

    template <typename TIterator, typename TComparator>
    void quick_sort_randomized_partition_internal(TIterator begin, TIterator end, std::mt19937& randomNumberGenerator)
    {
        auto size = std::distance(begin, end);

        if (size > 1)
        {
            std::uniform_int_distribution<decltype(end - begin)> uint_dist(0, size - 1); // generates numbers in range [0, size - 1]
            auto pivot = partition<TIterator, TComparator>(begin, end, begin + uint_dist(randomNumberGenerator));
            quick_sort_randomized_partition_internal<TIterator, TComparator>(begin, pivot, randomNumberGenerator);
            quick_sort_randomized_partition_internal<TIterator, TComparator>(std::next(pivot), end, randomNumberGenerator);
        }
    }

    /**
     * @brief Sort specified range of values using quick sort algorithm with randomized partitioning scheme.
     *
     * @tparam TIterator Iterator type (must be a random access iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThanOrEqualTo(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void quick_sort_randomized_partition(TIterator begin, TIterator end)
    {
        // Check that TIterator is a random-access iterator
        static_assert(
            std::is_same<typename std::iterator_traits<TIterator>::iterator_category,
            std::random_access_iterator_tag>::value,
            "TIterator must be a random-access iterator.");

        // Check for the existence of a member function "LessThanOrEqualTo" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThanOrEqualTo),
            bool (*)(const typename TIterator::value_type&, const typename TIterator::value_type&)>::value,
            "LessThanOrEqualTo function is missing in TComparator");

        auto size = end - begin;

        if (size > 1)
        {
            std::random_device rng;  // true random number generator
            std::mt19937 prng;       // pseudo-random number generator
            prng.seed(rng());        // seed prng with a number generated by rng

            quick_sort_randomized_partition_internal<TIterator, TComparator>(begin, end, prng);
        }
    }
}
