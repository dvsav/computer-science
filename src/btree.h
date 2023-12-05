#pragma once

#include "binary_search.h" // for cs::BinarySearch
#include "requires.h"      // for Requires
#include "utility.h"       // for DefaultComparator

#include <algorithm>       // for std::binary_search
#include <cmath>           // for ceilf
#include <tuple>           // for std::tie
#include <vector>          // for std:::vector
#include <utility>         // for std::pair

namespace cs
{
    /**
     * @class BTree
     * 
     * @brief Class representing a B-Tree - a data structure facilitating search of key-value pairs
     * and generalizing the idea of binary search tree (BST). Each node of a BST divides (separates)
     * the elements of its subtrees into two groups: those less than the node and those greater than
     * the node. So, BST node contains exactly one such separator. In contrast, a B-Tree node contains
     * more than one separator (in a sorted collection) and more than two subtrees.
     * 
     * @tparam Order the order of B-Tree: the max number of keys (separators) in a tree node is (Order - 1),
     * the max number of subtrees of a tree node is Order.
     * @tparam K type of a key which uniquely identifies a tree node.
     * @tparam V value of a tree node.
     * @tparam TComparator comparator type for comparing the keys
     * (must have a static member function bool LessThan(const K& a, const K& b)
     * where K is the key type. For example @see DefaultComparator, @see ReverseComparator.
     */
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
            root = nullptr;
        }

    public:
        /**
         * @brief Returns a pointer to the root of the tree.
         * @return Pointer to the root of the tree.
         */
        tree_node* Root() { return root; }
        /**
         * @brief Returns a pointer to the root of the tree.
         * @return Pointer to the root of the tree.
         */
        const tree_node* Root() const { return root; }

        /**
         * @brief Searches for a @p key in the B-Tree.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * 
         * @param key - the key to search for.
         * @return iterator containing the pointer to a tree node containing the
         * key searched for and the index of that key inside the node in case the key is found.
         * In case the key is not found, a default iterator is returned.
         */
        iterator find(const K& key)
        {
            if(root->IsEmpty())
                return iterator();

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

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node or key doesn't exists,
         * it's insersted in the tree.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& operator[](const K& key)
        {
            iterator it = insert(key, V()).first;
            return it.value();
        }

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node or key doesn't exists,
         * an exception is thrown.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& at(const K& key)
        {
            iterator it = find(key);
            Requires::That(it, FUNCTION_INFO);
            return it.value();
        }

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node or key doesn't exists,
         * an exception is thrown.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        const V& at(const K& key) const
        {
            iterator it = find(key);
            Requires::That(it, FUNCTION_INFO);
            return it.value();
        }

        /**
         * @brief Inserts a new key-value pair in the tree using specified @p key and @p value
         * and returns a pair (iterator-pointing-to-inserted-pair, true).
         * If such pair already exists, no action is taken and function returns
         * a pair (iterator-pointing-to-existing-pair, false).
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param key
         * @param value
         * @return a pair in which the first element is the iterator pointing to the key-value pair
         * and the second element is a boolean flag indicating whether the insertion was successful.
         * If a new key-value pair is created, fuction returns (pointer-to-new-pair, true).
         * If there is an existing pair which has specified @p key,
         * function returns (pointer-to-existing-pair, false).
         */
        std::pair<iterator, bool> insert(
            const K& key,
            const V& value)
        {
            tree_node* current_node = root;

            // Find a leaf to insert the new key-value pair into.
            // If there is an existing key-value pair with specified key, don't do the insertion.
            // If a leaf to insert to is overfilled, go to next step (throw up a median key from that node).
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

            // Throw up a median key from the overfilled leaf node splitting the node into two new nodes.
            // Continue throwing up median keys until either the current node is not overfilled or the
            // current node is the root.
            while (true)
            {
                tree_node* left = nullptr;
                tree_node* right = nullptr;
                tree_node* parent = current_node->Parent();
                // Extract median key (thrown_element) from the overfilled node
                // and split it into two new nodes (left and right)
                std::pair<K, V> thrown_element = current_node->Split(&left, &right);
                delete current_node;

                if (parent)
                {
                    // Remove the deleted node from its former parent
                    parent->ReplaceChild(current_node, nullptr);

                    // Find the position in the parent node at which to put the thrown key
                    size_t index_of_item = parent->FindItem(/*key*/ thrown_element.first).first;

                    // Insert the thrown key into the parent node
                    parent->InsertItem(
                        /*index*/ index_of_item,
                        /*key*/ thrown_element.first,
                        /*value*/ thrown_element.second);

                    // Add the 'left' node as a child to the parent
                    parent->setChild(
                        /*index*/ index_of_item,
                        /*child*/ left);

                    // Add the 'right' node as a child to the parent
                    parent->setChild(
                        /*index*/ index_of_item + 1,
                        /*child*/ right);

                    // If the parent is now overfilled, proceed with this loop (throwing up a median key)
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
                    // If current node (which is already split) is the root, create
                    // a new root consisting of just one key and two children (left and right)
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

        /**
         * @brief Removes from the tree a key-value pair pointed to by specified iterator @p it.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param it - iterator pointing to the key-value pair to be removed.
         * @return true if iterator @p it points to an existing key-value pair, false otherwise.
         */
        bool remove(const iterator& it)
        {
            if (!it)
                return false;

            tree_node* node_to_delete_from = it.node;
            size_t index_of_deleted_item = it.Index();

            tree_node* node_to_rebalance = nullptr;

            if (node_to_delete_from->IsLeaf())
            {
                // If the node to delete from is a leaf, remove the key from it...
                node_to_delete_from->RemoveItem(index_of_deleted_item);

                // ...after which the node may become underfilled,
                // so we'll need to rebalance the tree to make it filled again.
                node_to_rebalance = node_to_delete_from;
            }
            else
            {
                // If the node to delete from is an internal node,
                // find a replacement for the deleted key

                // Try to find in-order predecessor of the deleted item (rightmost node in the left subtree)
                iterator in_order_predecessor = node_to_delete_from->InOrderPredecessor(index_of_deleted_item);

                if (in_order_predecessor)
                {
                    // Replacement found

                    // Replace
                    node_to_delete_from->setItem(
                        /*index*/ index_of_deleted_item,
                        /*item*/ in_order_predecessor.node->getItem(in_order_predecessor.Index())
                    );

                    // Remove the replacement from its leaf node
                    in_order_predecessor.node->RemoveItem(in_order_predecessor.Index());

                    // After the deletion the 'in-order predecessor node' might become underfilled,
                    // so we'll need to rebalance the tree to make it filled again.
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
                            /*item*/ in_order_successor.node->getItem(in_order_successor.Index())
                        );

                        // Remove the replacement from its leaf node
                        in_order_successor.node->RemoveItem(in_order_successor.Index());

                        // After the deletion the 'in-order successor node' might become underfilled,
                        // so we'll need to rebalance the tree to make it filled again.
                        node_to_rebalance = in_order_successor.node;
                    }
                    else
                    {
                        // A node is either a leaf or it has an in-order predecessor
                        // or it has an in-order successor, so we shouldn't have gotten here
                        Requires::That(false, FUNCTION_INFO);
                    }
                }
            }

            // Rebalance the tree if needed
            Rebalance(node_to_rebalance);

            return true;
        }

        /**
         * @brief Removes a key-value pair having specified @p key from the tree.
         * Complexity: O( log(M, N) * log(M) ) where M is the max number of keys
         * in a tree node, N is the total number of elements in the tree.
         * @param key
         * @return true if a key-value pair with specified @p key was
         * found in the tree and false otherwise.
         */
        bool remove(const K& key)
        {
            return remove(find(key));
        }

        /**
         * @brief Prints out the tree in a pretty form.
         * @param os - output stream.
         */
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
                left_separator.setKey(left_sibling->items.back().first);
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
                right_separator.setKey(right_sibling->items.front().first);
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
                if (separator.node->ItemsNumber() == 1)
                    separator.node->setChild(separator.Index(), nullptr);
                separator.node->setChild(separator.Index() + 1, nullptr);
                separator.node->RemoveItem(separator.Index());
                delete right_node;

                // If the parent is the root and now has no elements, then free it and make the merged node the new root (tree becomes shallower)
                if (separator.node->IsRoot() && separator.node->IsEmpty())
                {
                    delete separator.node;
                    left_node->setParent(nullptr);
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

    /**
     * @class iterator
     * @brief Class representing an iterator pointing to a key-value pair in a B-Tree.
     * @see BTree
     * @tparam Order the order of B-Tree.
     * @tparam K type of a key which uniquely identifies a tree node.
     * @tparam V value of a tree node.
     * @tparam TComparator comparator type for comparing the keys.
     */
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

        BTreeNode* Node() { return node; }
        const BTreeNode* Node() const { return node; }

        size_t Index() const { return index; }

        const K& key() const { return node->getItem(index).first; }

        V& value() { return node->getItem(index).second; }
        const V& value() const { return node->getItem(index).second; }

        bool operator==(const iterator& other) const { return node == other.node && index == other.index; }
        bool operator!=(const iterator& other) const { return node != other.node || index != other.index; }

        operator bool() const { return node != nullptr; }

    private:
        void setKey(const K& key) { node->getItem(index).first = key; }
    };

    /**
     * @class BTreeNode
     * @brief Class representing a node of a B-Tree.
     * May contain multiple keys (in a sorted collection) and children/subtrees.
     * @see BTree
     * @tparam Order the order of B-Tree.
     * @tparam K type of a key which uniquely identifies a tree node.
     * @tparam V value of a tree node.
     * @tparam TComparator comparator type for comparing the keys.
     */
    template <size_t Order, typename K, typename V, typename TComparator>
    class BTree<Order, K, V, TComparator>::BTreeNode
    {
        friend class BTree;

    public:
        /**
         * @brief Returns the order of B-Tree.
         */
        static constexpr size_t order() { return Order; }
        /**
         * @brief Returns minimal number of keys in an internal node.
         */
        static constexpr size_t MinItems() { return CeilDivision<Order, 2>::value - 1; }
        /**
         * @brief Returns maximal number of keys in a node.
         */
        static constexpr size_t MaxItems() { return Order - 1; }
        /**
         * @brief Returns minimal number of children in an internal node.
         */
        static constexpr size_t MinChildren() { return MinItems() + 1; }
        /**
         * @brief Returns maximal number of children in a node.
         */
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
        /**
         * @brief Returns a pointer to the parent of this node.
         */
        BTreeNode* Parent() { return parent; }
        /**
         * @brief Returns a pointer to the parent of this node.
         */
        const BTreeNode* Parent() const { return parent; }

        /**
         * @brief Returns the number of keys in the node.
         */
        size_t ItemsNumber() const { return items.size(); }
        std::pair<K, V>& getItem(size_t index) { return items[index]; }
        const std::pair<K, V>& getItem(size_t index) const { return items[index]; }

        const K& Key(size_t index) const { return getItem(index).first; }

        V& Value(size_t index) { return getItem(index).second; }
        const V& Value(size_t index) const { return getItem(index).second; }

        /**
         * @brief Returns the number of children of the node.
         */
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
            return BinarySearch<std::pair<K, V>, K>(
                /*sortedArray*/ items,
                /*target*/ key,
                /*extractKey*/ [](const std::pair<K, V>& x) { return x.first; }
            );
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
            int right_separator_index = child_index;
            if (right_separator_index + 1 == static_cast<int>(parent->ChildrenNumber()))
            {
                *right_sibling = nullptr;
                return BTree::iterator();
            }
            else
            {
                *right_sibling = parent->children[right_separator_index + 1];
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
