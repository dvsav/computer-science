#pragma once

#include "requires.h"  // for Requires
#include "utility.h"   // for DefaultComparator

#include <algorithm>   // for std::find_if
#include <cmath>       // for ceilf
#include <tuple>       // for std::tie
#include <vector>      // for std:::vector
#include <utility>     // for std::pair

namespace cs
{
    template <size_t Order, typename K, typename V, typename TComparator = DefaultComparator<K> >
    class BTree
    {
        // Check that B-Tree order is greater than 2 (otherwise it won't be possible to split a tree node)
        static_assert(Order > 2, "Order must be greater than 2");

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
        class BTreeNode;
        class iterator;

    public:
        using tree_node = BTreeNode;
        using key_type = K;
        using value_type = V;
        static constexpr size_t order() { return Order; }

    private:
        tree_node* root;

    public:
        BTree() :
            root(new tree_node(/*parent*/ nullptr))
        {}

        ~BTree()
        {
            // delete every node in the tree
            DeleteTree(root);
        }

    public:
        iterator find(const K& key)
        {
            tree_node* current_node = root;

            while (current_node)
            {
                size_t index_of_item = 0;
                bool item_found = false;
                std::tie(index_of_item, item_found) = current_node->FindItem(key);
                if (item_found)
                    return iterator(current_node, index_of_item);
                else
                    current_node = current_node->getChild(index_of_item);
            }

            return iterator();
        }

        V& at(const K& key)
        {
            iterator it = find(key);
            Requires::That(it, FUNCTION_INFO);
            return it.value();
        }

        std::pair<iterator, bool> insert(
            const K& key,
            const V& value)
        {
            tree_node* current_node = root;

            while (true)
            {
                size_t index_of_item = 0;
                bool item_found = false;
                std::tie(index_of_item, item_found) = current_node->FindItem(key);

                if (item_found)
                {
                    // Item with this key is found, no insertion happens
                    return std::make_pair(
                        iterator(current_node, index_of_item),
                        /*inserted_new*/ false);
                }

                // Item with this key not found

                if (!current_node->IsLeaf())
                {
                    // current_node is not a leaf, so let's try to insert to its child
                    current_node = current_node->getChild(index_of_item);
                    continue;
                }

                // current_node is a leaf

                if (!current_node->IsFull())
                {
                    // There's enough room for a new element
                    current_node->InsertItem(
                        /*index*/ index_of_item,
                        /*key*/ key,
                        /*value*/ value);

                    return std::make_pair(
                        iterator(current_node, index_of_item),
                        /*inserted_new*/ true);
                }

                // current_node is full so we need to throw up one of its elements
                // which we are going to do ouside of this loop, but before...
                
                // ...we insert one extra element into the current_node,
                // so it becomes overfilled
                current_node->InsertItem(
                    /*index*/ index_of_item,
                    /*key*/ key,
                    /*value*/ value);
                break;
            }

            while (true)
            {
                // throw up one of the elements of current_node
                tree_node* left = nullptr;
                tree_node* right = nullptr;
                tree_node* parent = current_node->Parent();
                std::pair<K, V> thrown_element = current_node->Split(&left, &right);
                delete current_node;

                if (parent)
                {
                    parent->ReplaceChild(current_node, nullptr);

                    size_t index_of_item = parent->FindItem(/*key*/ thrown_element.first).first;

                    parent->InsertItem(
                        /*index*/ index_of_item,
                        /*key*/ thrown_element.first,
                        /*value*/ thrown_element.second);

                    parent->setChild(
                        /*index*/ index_of_item,
                        /*child*/ left);

                    parent->setChild(
                        /*index*/ index_of_item + 1,
                        /*child*/ right);

                    if (parent->IsOverfilled())
                    {
                        current_node = parent;
                        continue;
                    }
                    else
                    {
                        return std::make_pair(
                            iterator(parent, index_of_item),
                            /*inserted_new*/ true);
                    }
                }
                else
                {
                    root = new tree_node(/*parent*/ nullptr);

                    root->InsertItem(
                        /*index*/ 0,
                        /*key*/ thrown_element.first,
                        /*value*/ thrown_element.second);

                    root->setChild(
                        /*index*/ 0,
                        /*child*/ left);

                    root->setChild(
                        /*index*/ 1,
                        /*child*/ right);

                    return std::make_pair(
                        iterator(root, 0),
                        /*inserted_new*/ true);
                }
            }
        }

        bool remove(const iterator& it)
        {
            if (!it)
                return false;

            tree_node* node_to_delete_from = it.node;
            size_t index_of_deleted_item = it.index;

            tree_node* node_to_rebalance = nullptr;

            if (node_to_delete_from->IsLeaf())
            {
                node_to_delete_from->RemoveItem(index_of_deleted_item);

                node_to_rebalance = node_to_delete_from;
            }
            else
            {
                // Find a replacement for the deleted item

                // Try to find in-order predecessor of the deleted item (rightmost node in the left subtree)
                iterator in_order_predecessor = node_to_delete_from->InOrderPredecessor(index_of_deleted_item);

                if (in_order_predecessor)
                {
                    // Replacement found

                    // Replace
                    node_to_delete_from->setItem(
                        /*index*/ index_of_deleted_item,
                        /*item*/ in_order_predecessor.node->getItem(in_order_predecessor.index)
                    );

                    // Remove the replacement from its leaf node
                    in_order_predecessor.node->RemoveItem(in_order_predecessor.index);

                    node_to_rebalance = in_order_predecessor.node;
                }
                else
                {
                    // In-order predecessor not found

                    // Find in-order successor of the deleted item (leftmost node in the right subtree)
                    iterator in_order_successor = node_to_delete_from->InOrderSuccessor(index_of_deleted_item);

                    if (in_order_successor)
                    {
                        // Replacement found

                        // Replace
                        node_to_delete_from->setItem(
                            /*index*/ index_of_deleted_item,
                            /*item*/ in_order_successor.node->getItem(in_order_successor.index)
                        );

                        // Remove the replacement from its leaf node
                        in_order_successor.node->RemoveItem(in_order_successor.index);

                        node_to_rebalance = in_order_successor.node;
                    }
                    else
                    {
                        // Replacement not found, remove 
                        node_to_delete_from->RemoveItem(index_of_deleted_item);

                        node_to_rebalance = node_to_delete_from;
                    }
                }

            }

            // Rebalance the tree if needed
            Rebalance(node_to_rebalance);

            return true;
        }

        bool remove(const K& key)
        {
            return remove(find(key));
        }

        void print(std::ostream& os) const
        {
            PrintTree</*IsRoot*/ true, /*IsLast*/ false>(os, root);
        }

    private:
        template <bool IsRoot, bool IsLast>
        static void PrintTree(
            std::ostream& os,
            const tree_node* root,
            const std::string& prefix = "")
        {
            os << prefix;
            if (!IsRoot)
                os << (IsLast ? "`--" : "|--");

            if (!root)
            {
                os << std::endl;
                return;
            }

            for (size_t i = 0; i < root->ItemsNumber(); i++)
            {
                os << '(' << root->Key(i) << ", " << root->Value(i) << ") ";
            }
            os << std::endl;

            for (size_t i = 0; i < root->ChildrenNumber() - 1; i++)
            {
                PrintTree</*IsRoot*/ false, /*IsLast*/ false>(
                    os,
                    root->getChild(i),
                    prefix + std::string(IsRoot ? "" : IsLast ? "   " : "|  ")
                );
            }
            if (root->ChildrenNumber() > 0)
            {
                PrintTree</*IsRoot*/ false, /*IsLast*/ true>(
                    os,
                    root->getChild(root->ChildrenNumber() - 1),
                    prefix + std::string(IsRoot ? "" : IsLast ? "   " : "|  ")
                );
            }
        }

        void Rebalance(tree_node* deficient_node)
        {
            if (!deficient_node->IsUnderfilled())
            {
                // No need to rebalance
                return;
            }

            tree_node* left_sibling = nullptr;
            tree_node* right_sibling = nullptr;
            iterator left_separator = deficient_node->LeftSibling(&left_sibling);
            iterator right_separator = deficient_node->RightSibling(&right_sibling);

            // if the left sibling exists and has more than the minimum number of elements, then rotate right
            if (left_sibling && left_sibling->HasAvailableItems())
            {
                // Rotate right

                // Copy the separator from the parent to the start of the deficient node
                // (the separator moves down; deficient node now has the minimum number of elements)
                deficient_node->InsertItem(
                    /*index*/ 0,
                    /*key*/ left_separator.key(),
                    /*value*/ left_separator.value());
                
                // Replace the separator in the parent with the last element of the left sibling
                // (left sibling loses one node but still has at least the minimum number of elements)
                left_separator.key() = left_sibling->items.back().first;
                left_separator.value() = left_sibling->items.back().second;
                left_sibling->RemoveItem(left_sibling->ItemsNumber() - 1);
            }
            // If the right sibling exists and has more than the minimum number of elements, then rotate left
            else if (right_sibling && right_sibling->HasAvailableItems())
            {
                // Rotate left

                // Copy the separator from the parent to the end of the deficient node
                // (the separator moves down; the deficient node now has the minimum number of elements)
                deficient_node->InsertItem(
                    /*index*/ deficient_node->ItemsNumber(),
                    /*key*/ right_separator.key(),
                    /*value*/ right_separator.value());

                // Replace the separator in the parent with the first element of the right sibling
                // (right sibling loses one node but still has at least the minimum number of elements)
                right_separator.key() = right_sibling->items.front().first;
                right_separator.value() = right_sibling->items.front().second;
                right_sibling->RemoveItem(0);
            }
            // if both immediate siblings have only the minimum number of elements, then merge with a sibling
            // sandwiching their separator taken off from their parent
            else
            {
                iterator separator;
                tree_node* left_node = nullptr;
                tree_node* right_node = nullptr;
                if (left_sibling)
                {
                    separator = left_separator;
                    left_node = left_sibling;
                    right_node = deficient_node;
                }
                else if (right_sibling)
                {
                    separator = right_separator;
                    left_node = deficient_node;
                    right_node = right_sibling;
                }
                else
                {
                    Requires::That(false, FUNCTION_INFO);
                }

                // Copy the separator to the end of the left node
                // (the left node may be the deficient node or it may be the sibling with the minimum number of elements)
                left_node->InsertItem(
                    /*index*/ left_node->ItemsNumber(),
                    /*key*/ separator.key(),
                    /*value*/ separator.value());

                // Move all elements from the right node to the left node
                // (the left node now has the maximum number of elements, and the right node – empty)
                for (size_t i = 0; i < right_node->ItemsNumber(); i++)
                {
                    left_node->InsertItem(
                        /*index*/ left_node->ItemsNumber(),
                        /*key*/ right_node->Key(i),
                        /*value*/ right_node->Value(i));
                }

                // Remove the separator from the parent along with its empty right child (the parent loses an element)
                separator.node->setChild(separator.index + 1, nullptr);
                separator.node->RemoveItem(separator.index);
                delete right_node;

                // If the parent is the root and now has no elements, then free it and make the merged node the new root (tree becomes shallower)
                if (separator.node->IsRoot() && separator.node->IsEmpty())
                {
                    delete separator.node;
                    root = left_node;
                }
                // Otherwise, if the parent has fewer than the required number of elements, then rebalance the parent
                else if (separator.node->IsUnderfilled())
                {
                    Rebalance(separator.node);
                }
            }
        }

        static void DeleteTree(tree_node* root)
        {
            if (!root)
                return;

            for (BTreeNode* child : root->children)
                DeleteTree(child);

            delete root;
        }
    };

    template <size_t Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::iterator
    {
        friend class BTree; // for BTree to access iterator's constructor

    private:
        BTreeNode* node;
        size_t index;

    private:
        iterator() :
            node(nullptr),
            index(0)
        {}

        iterator(
            BTreeNode* node,
            size_t index) :
            node(node),
            index(index)
        {}

    public:
        const std::pair<K, V>& operator*() const { return node->getItem(index); }

        const K& key() const { return node->getItem(index).first; }

        V& value() { return node->getItem(index).second; }
        const V& value() const { return node->getItem(index).second; }

        bool operator==(const iterator& other)
        {
            return node == other.node && index == other.index;
        }

        bool operator!=(const iterator& other)
        {
            return node != other.node || index != other.index;
        }

        operator bool() const { return node != nullptr; }

    private:
        K& key() { return node->getItem(index).first; }
    };

    template <size_t Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::BTreeNode
    {
        friend class BTree;

    public:
        static constexpr size_t order() { return Order; }
        static constexpr size_t MinItems() { return CeilDivision<Order, 2>::value - 1; }
        static constexpr size_t MaxItems() { return Order - 1; }
        static constexpr size_t MinChildren() { return MinItems() + 1; }
        static constexpr size_t MaxChildren() { return MaxItems() + 1; }

    private:
        BTreeNode* parent;
        std::vector<BTreeNode*> children;
        std::vector< std::pair<K, V> > items;

    private:
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

        size_t ItemsNumber() const { return items.size(); }
        std::pair<K, V>& getItem(size_t index) { return items[index]; }
        const std::pair<K, V>& getItem(size_t index) const { return items[index]; }

        const K& Key(size_t index) const { return getItem(index).first; }

        V& Value(size_t index) { return getItem(index).second; }
        const V& Value(size_t index) const { return getItem(index).second; }

        size_t ChildrenNumber() const { return children.size(); }
        BTreeNode* getChild(size_t index) { return children[index]; }
        const BTreeNode* getChild(size_t index) const { return children[index]; }

    public:
        bool IsEmpty() const
        {
            return ItemsNumber() == 0;
        }

        bool IsRoot() const
        {
            return parent == nullptr;
        }

        bool IsLeaf() const
        {
            for (size_t i = 0; i < ChildrenNumber(); i++)
            {
                if (getChild(i) != nullptr)
                    return false;
            }
            return true;
        }

        bool IsFull() const
        {
            return (ItemsNumber() >= MaxItems());
        }

        bool IsOverfilled() const
        {
            return (ItemsNumber() > MaxItems());
        }

        bool IsUnderfilled() const
        {
            return !IsRoot() && (ItemsNumber() < MinItems());
        }

        bool HasAvailableItems() const
        {
            return ItemsNumber() > MinItems();
        }

        std::pair<size_t, bool> FindItem(const K& key) const
        {
            for (size_t i = 0; i < ItemsNumber(); i++)
            {
                if (TComparator::LessThan(key, Key(i)))
                    return std::make_pair(i, /*found*/ false);
                
                if (TComparator::EqualTo(Key(i), key))
                    return std::make_pair(i, /*found*/ true);
            }
            return std::make_pair(ItemsNumber(), /*found*/ false);
        }

        // Returns the rightmost item of the left subtree of 'this' node
        BTree::iterator InOrderPredecessor(size_t index) const
        {
            BTreeNode* current_node = children[index];
            if (!current_node)
                return BTree::iterator();

            while (true)
            {
                if (current_node->IsLeaf())
                    return BTree::iterator(current_node, current_node->ItemsNumber() - 1);
                else
                    current_node = current_node->children.back();
            }
        }

        // Returns the leftmost item of the right subtree of 'this' node
        BTree::iterator InOrderSuccessor(size_t index) const
        {
            BTreeNode* current_node = children[index + 1];
            if (!current_node)
                return BTree::iterator();

            while (true)
            {
                if (current_node->IsLeaf())
                    return BTree::iterator(current_node, 0);
                else
                    current_node = current_node->children.front();
            }
        }

        BTree::iterator LeftSibling(BTreeNode** left_sibling) const
        {
            if (this->IsRoot())
            {
                *left_sibling = nullptr;
                return BTree::iterator();
            }

            auto iter = std::find(parent->children.begin(), parent->children.end(), this);
            Requires::That(iter != parent->children.end(), FUNCTION_INFO);

            int child_index = iter - parent->children.begin();
            int left_separator_index = child_index - 1;
            if (left_separator_index < 0)
            {
                *left_sibling = nullptr;
                return BTree::iterator();
            }
            else
            {
                *left_sibling = parent->children[left_separator_index];
                return BTree::iterator(parent, left_separator_index);
            }
        }

        BTree::iterator RightSibling(BTreeNode** right_sibling) const
        {
            if (this->IsRoot())
            {
                *right_sibling = nullptr;
                return BTree::iterator();
            }

            auto iter = std::find(parent->children.begin(), parent->children.end(), this);
            Requires::That(iter != parent->children.end(), FUNCTION_INFO);

            int child_index = iter - parent->children.begin();
            int right_separator_index = child_index + 1;
            if (right_separator_index == static_cast<int>(parent->ChildrenNumber()))
            {
                *right_sibling = nullptr;
                return BTree::iterator();
            }
            else
            {
                *right_sibling = parent->children[right_separator_index];
                return BTree::iterator(parent, right_separator_index);
            }
        }

    private:
        void setParent(BTreeNode* node)
        {
            parent = node;
        }

        void setItem(size_t index, const std::pair<K, V>& item)
        {
            items[index] = item;
        }

        void setChild(size_t index, BTreeNode* child)
        {
            children[index] = child;
            if (child)
                child->setParent(this);
        }

        void ReplaceChild(
            BTreeNode* oldChild,
            BTreeNode* newChild)
        {
            std::replace(
                children.begin(), children.end(),
                oldChild, newChild);
        }

        void InsertItem(
            size_t index,
            const K& key,
            const V& value)
        {
            Requires::That(ItemsNumber() <= MaxItems(), FUNCTION_INFO);

            if (ItemsNumber() == 0)
            {
                items.emplace_back(key, value);
                children.push_back(nullptr);
                children.push_back(nullptr);
            }
            else
            {
                items.insert(
                    items.begin() + index,
                    std::make_pair(key, value)
                );

                children.insert(
                    children.begin() + index,
                    nullptr
                );
            }
        }

        void RemoveItem(size_t index)
        {
            Requires::That(index < ItemsNumber(), FUNCTION_INFO);

            if (ItemsNumber() == 1)
            {
                items.clear();
                Requires::That(!children[0] && !children[1], FUNCTION_INFO);
                children.clear();
            }
            else
            {
                items.erase(items.begin() + index);

                if (!children[index])
                {
                    children.erase(children.begin() + index);
                }
                else if (!children[index + 1])
                {
                    children.erase(children.begin() + index + 1);
                }
                else
                {
                    // Both left and right children aren't null, so can't delete
                    Requires::That(false, FUNCTION_INFO);
                }
            }
        }

        std::pair<K, V> Split(
            /*out*/ BTreeNode** left,
            /*out*/ BTreeNode** right)
        {
            const size_t thrown_index = Order / 2;
            std::pair<K, V> thrown_element = items[thrown_index];

            *left = new BTreeNode(parent);
            for (size_t i = 0; i < thrown_index; i++)
            {
                (*left)->InsertItem(i, Key(i), Value(i));
                (*left)->setChild(i, getChild(i));
            }
            (*left)->setChild(thrown_index, getChild(thrown_index));

            *right = new BTreeNode(parent);
            for (size_t i = thrown_index + 1; i < ItemsNumber(); i++)
            {
                (*right)->InsertItem(i - (thrown_index + 1), Key(i), Value(i));
                (*right)->setChild(i - (thrown_index + 1), getChild(i));
            }
            (*right)->setChild(items.size() - (thrown_index + 1), children.back());

            return thrown_element;
        }
    };
} // namespace cs
