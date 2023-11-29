#pragma once

#include "requires.h"  // for Requires
#include "utility.h"   // for DefaultComparator

#include <algorithm>   // for std::find_if
#include <vector>      // for std:::vector
#include <utility>     // for std::pair

namespace cs
{
    template <size_t Order, typename K, typename V, typename TComparator = DefaultComparator<K> >
    class BTree
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
        class BTreeNode;

    public:
        class iterator;

    private:
        BTreeNode* root;

    public:
        BTree() :
            root(new BTreeNode())
        {}

        ~BTree()
        {
            // delete every node in the tree
            root->Delete();
        }

    public:
        std::pair<iterator, bool> insert(
            const K& key,
            const V& value)
        {
            BTreeNode* current_node = root;

            while (true)
            {
                std::pair<size_t, bool> search_result = current_node->find(key);
                if (search_result.second)
                {
                    // Item with this key is found
                    return std::make_pair(
                        iterator(current_node, search_result.first),
                        /*inserted_new*/ false);
                }

                // Item with this key not found

                if (!current_node->IsFull())
                {
                    // There's enough room for a new element
                    current_node->insert(
                        /*index*/ search_result.first,
                        /*key*/ key,
                        /*value*/ value);

                    return std::make_pair(
                        iterator(current_node, iter.first),
                        /*inserted_new*/ true);
                }

                // current_node is full

                if (!current_node->IsLeaf())
                {
                    // current_node is not a leaf, so let's try to insert to its child
                    current_node = current_node->getChild(iter.first);
                    continue;
                }

                // current_node is a leaf, so we need to throw up one of its elements
                // which we are going to do ouside of this loop, but before...
                
                // ...As a quirk, we insert one extra element into the current_node,
                // because the node is going to be broken into two new nodes anyway
                current_node->insert(
                    /*index*/ iter.first,
                    /*key*/ key,
                    /*value*/ value);
                break;
            }

            while (true)
            {
                // throw up one of the elements of current_node
                BTreeNode* left = nullptr;
                BTreeNode* right = nullptr;
                BTreeNode* parent = current_node->Parent();
                std::pair<K, V> thrown_element = current_node->Split(&left, &right);
                delete current_node;
                if (parent)
                {
                    parent->ReplaceChild(current_node, nullptr);
                    current_node = parent;
                    std::pair<size_t, bool> search_result = current_node->find(thrown_element.first);
                    current_node->insert(
                        /*index*/ search_result.first,
                        /*key*/ thrown_element.first,
                        /*value*/ thrown_element.second);
                    root->setChild(search_result.first, left);
                    root->setChild(search_result.first + 1, right);
                }
                else
                {
                    root = new BTreeNode();
                    root->insert(0, key, value);
                    root->setChild(0, left);
                    root->setChild(1, right);
                    return std::make_pair(
                        iterator(root, 0),
                        /*inserted_new*/ true);
                    break;
                }
            }
        }
    };

    template <size_t Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::iterator
    {
    private:
        BTreeNode* node;
        size_t index;

    private:
        iterator(
            BTreeNode* node,
            size_t index) :
            node(node),
            index(index)
        {}

    public:
        std::pair<K, V>& operator*() { return node->getItem(index); }
        const std::pair<K, V>& operator*() const { return node->getItem(index); }
    };

    template <size_t Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::BTreeNode
    {
        static constexpr size_t MaxItems() { return Order - 1; }
        static constexpr size_t MaxChildren() { return Order; }

    private:
        BTreeNode* parent;
        std::vector<BTreeNode*> children;
        std::vector< std::pair<K, V> > items;

    public:
        BTreeNode(BTreeNode* parent) :
            parent(parent),
            children(),
            items()
        {
            items.reserve(Order);
            children.reserve(Order);
        }

        BTreeNode(const BTreeNode&) = delete;
        BTreeNode& operator=(const BTreeNode&) = delete;

    public:
        BTreeNode* Parent() { return parent; }
        const BTreeNode* Parent() const { return parent; }
        void setParent(BTreeNode* node) { parent = node; }

        size_t ItemsNumber() const { return items.size(); }
        std::pair<K, V>& getItem(size_t index) { return items[index]; }
        const std::pair<K, V>& getItem(size_t index) const { return items[index]; }

        size_t ChildrenNumber() const { return children.size(); }
        BTreeNode* getChild(size_t index) { return children[index]; }
        const BTreeNode* getChild(size_t index) const { return children[index]; }
        void setChild(size_t index, BTreeNode* node) { children[index] = node; }

        bool IsLeaf() const
        {
            return
                std::find_if(
                    children.cbegin(), children.cend(),
                    [](const BTreeNode* child) { return child != nullptr; }) == children.cend();
        }

        bool IsFull() const
        {
            return (items.size() >= MaxItems());
        }

        std::pair<size_t, bool> find(const K& key)
        {
            size_t i = 0;
            for (; i < items.size(); i++)
            {
                if (TComparator::LessThan(key, items[i]))
                    continue;
                else if (TComparator::EqualTo(key, items[i]))
                    return std::make_pair(i, /*found*/ true);
                else
                    break;
            }
            return std::make_pair(i, /*found*/ false);
        }

        void insert(
            size_t index,
            const K& key,
            const V& value)
        {
            Requires::That(items.size() < MaxItems() + 1, FUNCTION_INFO);

            items.insert(
                items.begin() + index,
                std::make_pair(key, value)
            );

            children.insert(
                children.begin() + index,
                nullptr
            );
        }

        void Delete()
        {
            for (BTreeNode* child : children)
            {
                if (child)
                    child->Delete();
            }
            delete this;
        }

        std::pair<K, V> Split(
            /*out*/ BTreeNode** left,
            /*out*/ BTreeNode** right)
        {
            const size_t thrown_index = Order / 2;
            std::pair<K, V> thrown_element = children[thrown_index];

            *left = new BTreeNode(parent);
            for (size_t i = 0; i < thrown_index; i++)
                (*left)->insert(children[i]);

            *right = new BTreeNode(parent);
            for (size_t i = thrown_index + 1; i < children.size(); i++)
                (*right)->insert(children[i]);

            return thrown_element;
        }
    };
} // namespace cs
