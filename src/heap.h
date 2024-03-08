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
    /**
     * @class Heap
     * @brief Class representing a heap (pyramid) data structure - a complete binary tree of elements.
     * Binary tree: every node has either 0, 1 or two children.
     * Complete tree: the tree is perfect (see below) up until the last level and all the leaves on the last level
     * are pushed to the left (completeness allows to store the tree in any indexed collection because
     * you can calculate the indices of children and parent of any node).
     * Perfect tree: all interior nodes have two children and all leaves are on the same level.
     * Heap always maintains either "min heap property" (children of any node are greater than their parent)
     * or "max heap property" (children of any node are less than their parent).
     * 
     * @tparam T type of tree element
     * @tparam TComparator Comparator type to compare tree elements
     * (must have a static member function bool LessThanOrEqualTo(const T& a, const T& b).
     * @tparam TContainer type of internal indexed collection.
     */
    template <typename T,
              typename TComparator,
              template <typename /*T*/, typename /*TAllocator*/> class TContainer = std::vector>
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
        using value_type = T;
        using const_iterator = typename container_type::const_iterator;

    private:
        container_type collection;

    public:
        /**
         * @brief Creates an empty heap.
         */
        Heap() :
            collection()
        {}

        /**
         * @brief Creates a heap filling it with values from list @p init_list.
         * Complexity: O(N) where N - number of elements in the heap.
         * 
         * @param init_list - initializer list of values.
         */
        Heap(std::initializer_list<T> init_list) :
            collection(init_list)
        {
            buildHeap();
        }

        /**
         * @brief Creates a heap filling it from a range of iterators.
         * Complexity: O(N) where N is the number of elements in the heap.
         *
         * @param begin - beginning of the range.
         * @param end - end of the range.
         * 
         * @tparam TIterator iterator type.
         */
        template <typename TIterator>
        Heap(TIterator begin, TIterator end) :
            collection(begin, end)
        {
            buildHeap();
        }

        Heap(const Heap&) = default;
        Heap(Heap&&) = default;

    public:
        Heap& operator=(const Heap&) = default;
        Heap& operator=(Heap&&) = default;

        /**
         * @brief Clears the heap.
         */
        void clear() { collection.clear(); }

        /**
         * @brief Returns true if the heap is empty and false otherwise.
         */
        bool empty() const { return collection.empty(); }

        /**
         * @brief Inserts a new element in the heap.
         * Complexity: O(lgN) where N is the number of elements in the heap.
         * 
         * @param value - the new element.
         */
        void insert(const T& value)
        {
            collection.push_back(value);
            heapifyUp(std::prev(collection.end()));
        }

        /**
         * @brief Returns the number of element in the heap.
         */
        size_t size() const { return collection.size(); }

        /**
         * @brief Returns the top (root) element of the heap.
         * For a min heap it's always the smallest element in the entire heap,
         * for a max heap it's always the greatest element in the entire heap.
         */
        const T& top() const
        {
            Requires::That(collection.size() > 0, "Collection size is zero", FUNCTION_INFO);
            return *collection.cbegin();
        }

        /**
         * @brief Removes the top element from the heap and reestablishes the heap property.
         * Complexity: O(lgN) where N is the number of elements in the heap.
         */
        void pop()
        {
            Requires::That(collection.size() > 0, "Collection size is zero", FUNCTION_INFO);
            *collection.begin() = std::move(*std::prev(collection.end()));
            collection.erase(std::prev(collection.end()));
            heapifyDown(collection.begin());
        }

        const_iterator begin() const { return collection.cbegin(); }

        const_iterator end() const { return collection.cend(); }

    private:
        // Returns the parent of specified element.
        iterator_type parent(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            return collection.begin() + ((dist + 1) / 2 - 1);
        }

        // Returns the left child of specified element.
        iterator_type leftChild(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            auto childDist = (dist + 1) * 2 - 1;
            return static_cast<size_t>(childDist) < collection.size() ?
                collection.begin() + childDist :
                collection.end();
        }

        // Returns the right child of specified element.
        iterator_type rightChild(iterator_type i)
        {
            auto dist = std::distance(collection.begin(), i);
            auto childDist = (dist + 1) * 2;
            return static_cast<size_t>(childDist) < collection.size() ?
                collection.begin() + childDist :
                collection.end();
        }

        // Bubbles up an element until it gets to its correct position in the heap.
        // Complexity: O(lgN) where N is the number of elements in the heap.
        void heapifyUp(iterator_type i)
        {
            if (i != collection.begin() && TComparator::LessThan(*i, *parent(i)))
            {
                std::swap(*i, *parent(i));
                heapifyUp(parent(i));
            }
        }

        // Sink down the element until it gets to its correct position in the heap.
        // Complexity: O(lgN) where N is the number of elements in the heap.
        void heapifyDown(iterator_type i)
        {
            iterator_type lChild = leftChild(i);
            if (lChild == collection.end()) return;

            iterator_type rChild = rightChild(i);

            iterator_type minChildIndex =
                (rChild != collection.end()) ?
                (TComparator::LessThan(*lChild, *rChild) ? lChild : rChild) :
                lChild;

            if (TComparator::LessThan(*minChildIndex, *i))
            {
                std::swap(*i, *minChildIndex);
                heapifyDown(minChildIndex);
            }
        }

        // Move the elements of collection around so to establish the heap property.
        void buildHeap()
        {
            // Begin from the penultimate level and go to the root, heapifying every element down.
            // Complexity: O(N) where N is the number of elements in the heap.
            if (collection.size() > 1)
            {
				// find the last element of penultimate level of the heap
				size_t k = 2;
				while (k - 2 < size() - 1)
					k *= 2;
				k = k / 2 - 2;
				
				// heapify down elements starting from last element of penultimate level all the way up to the root of the heap
                for (auto i = collection.begin() + k; i != collection.begin(); --i)
                    heapifyDown(i);
                heapifyDown(collection.begin());
            }
        }
    };

    template <typename T, template <typename, typename> class TContainer = std::vector>
    using MinHeap = Heap<T, DefaultComparator<T>, TContainer>;

    template <typename T, template <typename, typename> class TContainer = std::vector>
    using MaxHeap = Heap<T, ReverseComparator<T>, TContainer>;

    /**
     * @brief Sort specified range of values using heap data structure.
     * Idea: create a heap out of an array, extract top element (which is always minimal in a min heap) iteratively.
     * Complexity: O(NlgN).
     *
     * @tparam TIterator Iterator type (must be a random access iterator).
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const T& a, const T& b)
     * where T is the value type of container being sorted). For example @see DefaultComparator, @see ReverseComparator.
     *
     * @param begin - Iterator specifying the beginning of a range to be sorted.
     * @param end - Iterator specifying the end of a range to be sorted.
     */
    template <typename TIterator, typename TComparator = DefaultComparator<typename TIterator::value_type> >
    void heap_sort(TIterator begin, TIterator end)
    {
        Heap<typename TIterator::value_type, TComparator> heap(begin, end);
        for (auto i = begin; i != end; i++)
        {
            *i = heap.top();
            heap.pop();
        }
    }
} // namespace cs
