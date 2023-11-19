#pragma once

#include "requires.h"       // for Requires::That
#include "utility.h"        // for DefaultComparator, ReverseComparator

#include <algorithm>        // for std::swap
#include <initializer_list> // for std::initializer_list
#include <iostream>         // for ostream
#include <iterator>         // for std::iterator_traits, std::distance
#include <type_traits>      // for std::is_same
#include <vector>           // for std::vector

namespace cs
{
    template <typename T, typename TComparator, template <typename, typename> class TContainer = std::vector>
    class Heap
    {
        using container_type = TContainer<T, std::allocator<T> >;
        using iterator_type = typename container_type::iterator;
        using iterator_category = typename std::iterator_traits<typename container_type::iterator>::iterator_category;

        // Check if the container's iterator category is random access
        static_assert(
            std::is_same<iterator_category, std::random_access_iterator_tag>::value,
            "Container must support random access!");

        // Check for the existence of a member function "LessThanOrEqualTo" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThanOrEqualTo),
            bool (*)(const typename container_type::value_type&, const typename container_type::value_type&)>::value,
            "LessThanOrEqualTo function is missing in TComparator");

    public:
        using iterator = typename container_type::const_iterator;

    private:
        container_type collection;

    public:
        Heap() :
            collection()
        {}

        Heap(std::initializer_list<T> init_list) :
            collection(init_list)
        {
            BuildHeap();
        }

        template <typename TIterator>
        Heap(TIterator begin, TIterator end) :
            collection(begin, end)
        {
            BuildHeap();
        }

        Heap(const Heap&) = default;
        Heap(Heap&&) = default;

    public:
        Heap& operator=(const Heap&) = default;
        Heap& operator=(Heap&&) = default;

        void Insert(const T& value)
        {
            collection.push_back(value);
            HeapifyUp(std::prev(collection.end()));
        }

        size_t Size() const { return collection.size(); }

        const T& Top() const
        {
            Requires::That(Size() > 0, "Collection size is zero", FUNCTION_INFO);
            return *collection.cbegin();
        }

        void Pop()
        {
            Requires::That(Size() > 0, "Collection size is zero", FUNCTION_INFO);
            *collection.begin() = *std::prev(collection.end());
            collection.erase(std::prev(collection.end()));
            HeapifyDown(collection.begin());
        }

        iterator begin() const { return collection.cbegin(); }

        iterator end() const { return collection.cend(); }

    private:
        iterator_type Parent(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            return collection.begin() + ((dist + 1) / 2 - 1);
        }

        iterator_type LeftChild(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            auto childDist = (dist + 1) * 2 - 1;
            return static_cast<size_t>(childDist) < collection.size() ?
                collection.begin() + childDist :
                collection.end();
        }

        iterator_type RightChild(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            auto childDist = (dist + 1) * 2;
            return static_cast<size_t>(childDist) < collection.size() ?
                collection.begin() + childDist :
                collection.end();
        }

        void HeapifyUp(iterator_type i)
        {
            if (i != collection.begin() && TComparator::LessThan(*i, *Parent(i)))
            {
                std::swap(*i, *Parent(i));
                HeapifyUp(Parent(i));
            }
        }

        void HeapifyDown(iterator_type i)
        {
            iterator_type lChild = LeftChild(i);
            if (lChild == collection.end()) return;

            iterator_type rChild = RightChild(i);

            iterator_type minChildIndex =
                (rChild != collection.end()) ?
                (TComparator::LessThan(*lChild, *rChild) ? lChild : rChild) :
                lChild;

            if (TComparator::LessThan(*minChildIndex, *i))
            {
                std::swap(*i, *minChildIndex);
                HeapifyDown(minChildIndex);
            }
        }

        void BuildHeap()
        {
            if (collection.size() > 0)
            {
                for (auto i = std::prev(collection.end()); i != collection.begin(); --i)
                    HeapifyDown(i);
                HeapifyDown(collection.begin());
            }
        }
    };

    template <typename T, template <typename, typename> class TContainer = std::vector>
    using MinHeap = Heap<T, DefaultComparator<T>, TContainer>;

    template <typename T, template <typename, typename> class TContainer = std::vector>
    using MaxHeap = Heap<T, ReverseComparator<T>, TContainer>;
} // namespace cs
