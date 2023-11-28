#pragma once

#include "requires.h"  // for Requires
#include "utility.h"   // for DefaultComparator

#include <vector>      // for std:::vector
#include <utility>     // for std::pair

namespace cs
{
    template <unsigned int Order, typename K, typename V, typename TComparator = DefaultComparator<K> >
    class BTree
    {
    private:
        class BTreeNode;
    };

    template <unsigned int Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::BTreeNode
    {
        // Check that B-Tree order is greater than 1
        static_assert(Order > 1);

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

    public:
        using key_type = K;
        using value_type = V;
        static constexpr unsigned int order() { return Order; }

    private:
        std::vector< std::pair<K, V> > items;

    public:
        BTreeNode() :
            items()
        {
            items.reserve(Order - 1);
        }
    };
} // namespace cs
