#pragma once

#include "requires.h"  // for Requires
#include "utility.h"   // for DefaultComparator

#include <cmath>       // for std::abs
#include <algorithm>   // for std::max
#include <functional>  // for std::function
#include <iostream>    // for std::ostream
#include <stack>       // for std::stack
#include <string>      // for std::string
#include <type_traits> // for std::is_same
#include <utility>     // for std:::pair

namespace cs
{
    /**
     * @class TreeNode
     * 
     * @brief Class representing a node in a tree (e.g. binary search tree).
     * Has 'key' and 'value' fields. 'key' is supposed to be used for binary search.
     * 
     * @tparam K key type.
     * @tparam V value type.
     */
    template <typename K, typename V>
    class TreeNode
    {
        template <typename, typename, typename>
        friend class BinarySearchTree;

        template <typename, typename, typename>
        friend class AvlTree;

    public:
        using key_type = K;
        using value_type = V;

    private:
        TreeNode* parent;
        TreeNode* left;
        TreeNode* right;
        K key;
        V value;

        // Cached value of the height of the tree rooted in this node.
        // This field is supposed to be used by the friend classes (trees).
        int height;

    public:
        /**
         * @brief Creates a tree node.
         * 
         * @param key
         * @param left - left child
         * @param right - right child
         */
        TreeNode(
            const K& key,
            TreeNode* left = nullptr,
            TreeNode* right = nullptr) :
            parent(nullptr),
            left(left),
            right(right),
            key(key),
            value(),
            height(0)
        {
            init();
        }

        /**
         * @brief Creates a tree node.
         *
         * @param key
         * @param value
         * @param left - left child
         * @param right - right child
         */
        TreeNode(
            const K& key,
            const V& value,
            TreeNode* left = nullptr,
            TreeNode* right = nullptr) :
            parent(nullptr),
            left(left),
            right(right),
            key(key),
            value(value),
            height(0)
        {
            init();
        }

        TreeNode(const TreeNode&) = delete;
        TreeNode& operator=(const TreeNode&) = delete;

    public:
        /**
         * @brief Returns a pointer to the left child of this node.
         * @return a pointer to the left child of this node.
         */
        const TreeNode* Left() const { return left; }
        /**
         * @brief Returns a pointer to the left child of this node.
         * @return a pointer to the left child of this node.
         */
        TreeNode* Left() { return left; }

        /**
         * @brief Returns a pointer to the right child of this node.
         * @return a pointer to the right child of this node.
         */
        const TreeNode* Right() const { return right; }
        /**
         * @brief Returns a pointer to the right child of this node.
         * @return a pointer to the right child of this node.
         */
        TreeNode* Right() { return right; }

        /**
         * @brief Returns a pointer to the parent of this node.
         * @return a pointer to the parent of this node.
         */
        const TreeNode* Parent() const { return parent; }
        /**
         * @brief Returns a pointer to the parent of this node.
         * @return a pointer to the parent of this node.
         */
        TreeNode* Parent() { return parent; }

        /**
         * @brief Returns a reference to the key of this node.
         * @return a reference to the key of this node.
         */
        const K& Key() const { return key; }

        /**
         * @brief Returns a reference to the value of this node.
         * @return a reference to the value of this node.
         */
        const V& Value() const { return value; }
        /**
         * @brief Returns a reference to the value of this node.
         * @return a reference to the value of this node.
         */
        V& Value() { return value; }

    private:
        // This method is called by constructors (to avoid code duplication).
        void init()
        {
            if (left)
                left->setParent(this);

            if (right)
                right->setParent(this);
        }

        // Sets the left child of this node.
        // This method is supposed to be called by the friend classes (trees).
        void setLeft(TreeNode* node) { left = node; }

        // Sets the right child of this node.
        // This method is supposed to be called by the friend classes (trees).
        void setRight(TreeNode* node) { right = node; }

        // Sets the parent of this node.
        // This method is supposed to be called by the friend classes (trees).
        void setParent(TreeNode* node) { parent = node; }

        // Returns the cached value of the height of the tree rooted in this node.
        // This method is supposed to be called by the friend classes (trees).
        int Height() const { return height; }

        // Returns the balance factor of this node.
        // Balance factor of a node is the difference between its right and the left subtrees.
        // This method is supposed to be called by the friend classes (trees).
        int BalanceFactor() const
        {
            return
                (this->Right() ? 1 + this->Right()->Height() : 0) -
                (this->Left() ? 1 + this->Left()->Height() : 0);
        }
    };

    /**
     * @brief Visits (calls @p visitor functor)
     * nodes of a tree in a pre-order fashion.
     * 
     * @param root - tree root.
     * @param visitor - functor called for each visited node.
     * 
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void PreOrderTraverse(
        TTreeNode* root,
        std::function<void(TTreeNode*)> visitor)
    {
        if (root)
        {
            visitor(root);
            PreOrderTraverse<TTreeNode>(root->Left(), visitor);
            PreOrderTraverse<TTreeNode>(root->Right(), visitor);
        }
    }

    /**
     * @brief Visits (calls @p visitor functor)
     * nodes of a tree in a in-order fashion.
     *
     * @param root - tree root.
     * @param visitor - functor called for each visited node.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void InOrderTraverse(
        TTreeNode* root,
        std::function<void(TTreeNode*)> visitor)
    {
        if (root)
        {
            InOrderTraverse<TTreeNode>(root->Left(), visitor);
            visitor(root);
            InOrderTraverse<TTreeNode>(root->Right(), visitor);
        }
    }

    /**
     * @brief Visits (calls @p visitor functor)
     * nodes of a tree in a post-order fashion.
     *
     * @param root - tree root.
     * @param visitor - functor called for each visited node.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void PostOrderTraverse(
        TTreeNode* root,
        std::function<void(TTreeNode*)> visitor)
    {
        if (root)
        {
            PostOrderTraverse<TTreeNode>(root->Left(), visitor);
            PostOrderTraverse<TTreeNode>(root->Right(), visitor);
            visitor(root);
        }
    }

    /**
     * @brief Visits (calls @p visitor functor)
     * nodes of a tree in a level-order fashion.
     *
     * @param root - tree root.
     * @param visitor - functor called for each visited node.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void LevelOrderTraverse(
        TTreeNode* root,
        std::function<void(TTreeNode*)> visitor)
    {
        if (!root)
            return;

        std::stack<TTreeNode*> level;
        level.push(root);

        while (!level.empty())
        {
            TTreeNode* node = level.top();
            level.pop();

            if (node->Left())
                level.push(node->Left());
            if (node->Right())
                level.push(node->Right());

            // Visit after we are done with node's children
            visitor(node);
        }
    }

    /**
     * @brief Deteles every node of specified tree.
     * @param root - tree root.
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void DeleteTree(
        TTreeNode* root)
    {
        // We've chosen post-order traverse because it
        // visits leaves first and goes towards the root.
        PostOrderTraverse<TTreeNode>(
            root,
            [](TTreeNode* node) -> void
            {
                delete node;
            });
    }

    /**
     * @brief Passes through all ancestors (parent, parent's parent, etc.)
     * of @p child (bottom-up) until either there are no more ancestors or @p predicate
     * is satisfied. Returns either the first ancestor satisfying @p predicate
     * or nullptr if there are no such ancestors.
     * 
     * @param child - node of a tree whose ancestors are looked through.
     * @param predicate - predicate to be satisfied.
     * @param inclusive - if true, includes @p node itself in the search.
     * 
     * @return either the first ancestor satisfying @p predicate or nullptr
     * if there are no such ancestors.
     * 
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    TTreeNode* FindAncestor(
        TTreeNode* child,
        std::function<bool(const TTreeNode*)> predicate,
        bool inclusive)
    {
        if (!child)
            return nullptr;

        TTreeNode* current_node = inclusive ? child : child->Parent();
        while (current_node && !predicate(current_node))
            current_node = current_node->Parent();
        return current_node;
    }

    /**
     * @brief Prints out specified tree in a pretty form.
     *
     * @param os - output stream.
     * @param root - tree root.
     * @param prefix
     * 
     * @tparam TTreeNode node type.
     * @tparam IsRoot
     * @tparam IsLeft
     */
    template <typename TTreeNode, bool IsRoot = true, bool IsLeft = false>
    void PrintTree(
        std::ostream& os,
        const TTreeNode* root,
        const std::string& prefix = "")
    {
        os << prefix;
        if (!IsRoot)
            os << (IsLeft ? "|--" : "`--");

        if (!root)
        {
            os << std::endl;
            return;
        }

        os << '(' << root->Key() << ", " << root->Value() << ')' << std::endl;

        if (!root->Left() && !root->Right())
            return;

        PrintTree<TTreeNode, false, true>(os, root->Left(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
        PrintTree<TTreeNode, false, false>(os, root->Right(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
    }

    /**
     * @brief Returns in-order predecessor of @p root node.
     * In-order predecessor of a tree node is the rightmost
     * element in its left subtree.
     * 
     * @param root - tree root.
     * 
     * @return in-order predecessor of @p root node.
     * 
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    TTreeNode* InOrderPredecessor(TTreeNode* root)
    {
        if (!root)
            return nullptr;

        TTreeNode* predecessor = root->Left();
        if (!predecessor)
            return nullptr;

        while (predecessor->Right())
            predecessor = predecessor->Right();

        return predecessor;
    }

    /**
     * @brief Calculates the height of a tree rooted in @p root node.
     * Height of a tree is the maximal number of edges in a path from
     * its root to any of its leaves.
     *
     * @param root - tree root.
     *
     * @return height of a tree rooted in @p root node.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    int Height(const TTreeNode* root)
    {
        return std::max(
            root->Right() ? 1 + Height(root->Right()) : 0,
            root->Left() ? 1 + Height(root->Left()) : 0);
    }

    /**
     * @brief Calculates the balance factor of a tree rooted in @p root node.
     * Balance factor of a tree is the difference of the right height of the tree
     * and the left height of the tree.
     *
     * @param root - tree root.
     *
     * @return balance factor of a tree rooted in @p root node. 0 - tree is balanced;
     * <0 - tree is biased to the left; >0 - tree is biased to the right.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    int BalanceFactor(const TTreeNode* root)
    {
        return
            (root->Right() ? 1 + Height(root->Right()) : 0) -
            (root->Left() ? 1 + Height(root->Left()) : 0);
    }

    /**
     * @brief Checks if specified tree is balanced or not.
     * A tree is balanced if the absolute value of the balance
     * factor of any of its nodes is less than 2.
     *
     * @param root - tree root.
     *
     * @return true if the tree is balanced, false otherwise.
     *
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    bool IsBalanced(const TTreeNode* root)
    {
        TTreeNode* unbalanced_node = nullptr;
        LevelOrderTraverse<TTreeNode>(
            const_cast<TTreeNode*>(root),
            [&unbalanced_node](TTreeNode* node) -> void
            {
                if (std::abs(cs::BalanceFactor(node)) > 1)
                    unbalanced_node = node;
            }
        );
        return (unbalanced_node == nullptr);
    }

    /**
     * @class BinarySearchTree
     * 
     * @brief Class representing a basic binary search tree.
     * Allows for adding and searching elements (key-value pairs).
     * This search tree is not trying to rebalance itself in case of unbalance,
     * so it's not supposed to be used other than be inherited by other classes
     * that do maintain balance in themselves.
     * 
     * @tparam K key type of a tree node.
     * @tparam V value type of a tree node.
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const K& a, const K& b)
     * where K is the key type. For example @see DefaultComparator, @see ReverseComparator.
    */
    template <typename K, typename V, typename TComparator = DefaultComparator<K> >
    class BinarySearchTree
    {
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
        using tree_node = TreeNode<K, V>;

    protected:
        tree_node* root;

    public:
        /**
         * @brief Creates an empty BinarySearchTree.
         */
        BinarySearchTree() :
            root(nullptr)
        {}

        // Move constructor.
        BinarySearchTree(BinarySearchTree&& rvalue) :
            root(rvalue.root)
        {
            rvalue.root = nullptr;
        }

        BinarySearchTree(const BinarySearchTree&) = delete;

        // The destructor is virtual because this class is supposed to be inherited.
        virtual ~BinarySearchTree()
        {
            DeleteTree(root);
            root = nullptr;
        }

    public:
        BinarySearchTree& operator=(const BinarySearchTree&) = delete;
        
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
         * @brief Returns the pointer to the tree node containing
         * specified @p key or nullptr if such node is not found.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key - the key to search for.
         * @return Pointer to the tree node containing specified @p key
         * or nullptr if such node is not found.
         */
        tree_node* find(const K& key)
        {
            tree_node* current_node = root;
            while (true)
            {
                if (!current_node)
                    return nullptr;

                if (TComparator::EqualTo(key, current_node->Key()))
                    return current_node;
                else if (TComparator::LessThan(key, current_node->Key()))
                    current_node = current_node->Left();
                else
                    current_node = current_node->Right();
            }
        }

        /**
         * @brief Returns the pointer to the tree node containing
         * specified @p key or nullptr if such node is not found.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key - the key to search for.
         * @return Pointer to the tree node containing specified @p key
         * or nullptr if such node is not found.
         */
        const tree_node* find(const K& key) const
        {
            tree_node* current_node = root;
            while (true)
            {
                if (!current_node)
                    return nullptr;

                if (TComparator::EqualTo(key, current_node->Key()))
                    return current_node;
                else if (TComparator::LessThan(key, current_node->Key()))
                    current_node = current_node->Left();
                else
                    current_node = current_node->Right();
            }
        }

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node doesn't exists,
         * it's insersted in the tree.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& operator[](const K& key)
        {
            return insert(key, V()).first->Value();
        }

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node doesn't exists,
         * an exception is thrown.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& at(const K& key)
        {
            tree_node* node = find(key);
            Requires::That(node, FUNCTION_INFO);
            return node->Value();
        }

        /**
         * @brief Returns a reference to a value of a tree node
         * having specified @p key. If such node doesn't exists,
         * an exception is thrown.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        const V& at(const K& key) const
        {
            tree_node* node = find(key);
            Requires::That(node, FUNCTION_INFO);
            return node->Value();
        }

        /**
         * @brief Inserts a new node in the tree having specified @p key and @p value
         * and returns a pair (pointer-to-new-node, true).
         * If such node already exists, no action is taken and function returns
         * a pair (pointer-to-existing-node, false).
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key
         * @param value
         * @return a pair in which the first element is the pointer to a tree node and the
         * second element is a boolean flag indicating whether the insertion was successful.
         * If a new tree node is created, fuction returns (pointer-to-new-node, true).
         * If there is an existing node which has specified @p key,
         * function returns (pointer-to-existing-node, false).
         */
        virtual std::pair<tree_node*, bool> insert(
            const K& key,
            const V& value)
        {
            // If the tree is empty, inserted node is going to be the root
            if (!root)
            {
                root = new tree_node(key, value);
                return std::make_pair(root, true);
            }

            // Find the right place to put the new node to (similar to find function)
            tree_node* current_node = root;
            while (true)
            {
                if (TComparator::EqualTo(key, current_node->Key()))
                {
                    // A node with such key already exists, return it
                    return std::make_pair(current_node, false);
                }
                else if (TComparator::LessThan(key, current_node->Key()))
                {
                    if (current_node->Left())
                    {
                        current_node = current_node->Left();
                    }
                    else
                    {
                        tree_node* new_node = new tree_node(key, value);
                        current_node->setLeft(new_node);
                        new_node->setParent(current_node);
                        refreshHeightUp(current_node);
                        return std::make_pair(new_node, true);
                    }
                }
                else
                {
                    if (current_node->Right())
                    {
                        current_node = current_node->Right();
                    }
                    else
                    {
                        tree_node* new_node = new tree_node(key, value);
                        current_node->setRight(new_node);
                        new_node->setParent(current_node);
                        refreshHeightUp(current_node);
                        return std::make_pair(new_node, true);
                    }
                }
            }
        }

        /**
         * @brief Removes a node having specified @p key from the tree.
         * Complexity: best case (balanced tree) - O(lgN), worst case - O(N),
         * where N is the number of nodes in the tree.
         * @param key
         * @return true if a node with specified @p key was found in the tree, false otherwise.
         */
        virtual bool remove(const K& key)
        {
            tree_node* node_removed = find(key);
            if (node_removed)
            {
                // The node to replace the node being removed is either its in-order predecessor or its right child
                tree_node* in_order_predecessor = InOrderPredecessor<tree_node>(node_removed);
                tree_node* replacement = in_order_predecessor ? in_order_predecessor : node_removed->Right();

                remove(node_removed, replacement);
                return true;
            }
            else
                return false;
        }

    protected:
        // Replaces 'node_removed' with a specified leaf node 'replacement'.
        // This operation may cause an imbalance and the function
        // returns the potential imbalance node.
        tree_node* remove(
            tree_node* node_removed,
            tree_node* replacement)
        {
            tree_node* parent = node_removed->Parent();

            tree_node* potential_imbalance_node = nullptr;
            if (replacement)
            {
                // Imbalance may occur either in the left subtree of 'node_removed' (if there's an in-order predecessor)
                // or in the 'replacement' node itself after it takes place of 'node_removed'
                potential_imbalance_node =
                    replacement->Parent() != node_removed ?
                    replacement->Parent() : replacement;
            }
            else
            {
                // Or if the 'node_removed' is a leaf, imbalance may occur in the grandparent of 'node_removed'.
                if (node_removed->Parent() && node_removed->Parent()->Parent())
                    potential_imbalance_node = node_removed->Parent()->Parent();
            }

            // Update 'height' all the way from 'potential_imbalance_node' to the root
            refreshHeightUp(potential_imbalance_node);

            // If replacement != nullptr, this means that we are trying to delete a non-leaf node
            if (replacement)
            {
                tree_node* left = node_removed->Left();
                tree_node* right = node_removed->Right();

                // Update replacement's parent's child pointer
                replacement->Parent()->setRight(nullptr);
                replacement->setParent(nullptr);

                // update replacement's children pointers
                if (replacement != left)
                {
                    replacement->setLeft(left);
                    if (left)
                        left->setParent(replacement);
                }

                if (replacement != right)
                {
                    replacement->setRight(right);
                    if (right)
                        right->setParent(replacement);
                }
            }

            // Update parent's child pointer
            if (parent)
            {
                if (parent->Left() == node_removed)
                    parent->setLeft(replacement);
                else if (parent->Right() == node_removed)
                    parent->setRight(replacement);
                else
                    Requires::That(false, FUNCTION_INFO);

                if (replacement)
                    replacement->setParent(parent);
            }
            else
                root = replacement;

            delete node_removed;

            return potential_imbalance_node;
        }

        // Updates 'height' for specified node only
        static void refreshHeight(TreeNode<K, V>* node)
        {
            node->height = std::max(
                node->Left() ? 1 + node->Left()->Height() : 0,
                node->Right() ? 1 + node->Right()->Height() : 0);
        }

        // Updates 'height' beginning from the specified node and all the way up to the root
        static void refreshHeightUp(TreeNode<K, V>* node)
        {
            tree_node* current_node = node;
            while (current_node)
            {
                refreshHeight(current_node);
                current_node = current_node->Parent();
            }
        }
    };

    /**
     * @class AvlTree
     *
     * @brief Class representing a balanced binary search tree.
     * This tree is distinguished by an algorithm of rebalancing after each insertion
     * or removal - AVL named after its authors: Adelson, Velsky and Landis.
     *
     * @tparam K key type of a tree node.
     * @tparam V value type of a tree node.
     * @tparam TComparator Comparator type (must have a static member function bool LessThan(const K& a, const K& b)
     * where K is the key type. For example @see DefaultComparator, @see ReverseComparator.
    */
    template <typename K, typename V, typename TComparator = DefaultComparator<K> >
    class AvlTree final : public BinarySearchTree<K, V, TComparator>
    {
    public:
        using base = BinarySearchTree<K, V, TComparator>;
        using key_type = typename base::key_type;
        using value_type = typename base::value_type;
        using tree_node = typename base::tree_node;

    public:
        // Use the constructors of base class.
        using base::BinarySearchTree;

    public:
        /**
         * @brief Inserts a new node in the tree having specified @p key and @p value
         * and returns a pair (pointer-to-new-node, true).
         * If such node already exists, no action is taken and function returns
         * a pair (pointer-to-existing-node, false).
         * Complexity: O(lgN), where N is the number of nodes in the tree.
         * @param key
         * @param value
         * @return a pair in which the first element is the pointer to a tree node and the
         * second element is a boolean flag indicating whether the insertion was successful.
         * If a new tree node is created, fuction returns (pointer-to-new-node, true).
         * If there is an existing node which has specified @p key,
         * function returns (pointer-to-existing-node, false).
         */
        std::pair<tree_node*, bool> insert(
            const K& key,
            const V& value) override
        {
            auto node_inserted = base::insert(key, value);
            if (node_inserted.second)
            {
                tree_node* new_node = node_inserted.first;

                // Find the deepest node out of balance
                tree_node* deepest_unbalanced = FindAncestor<tree_node>(
                    new_node,
                    [](const tree_node* ancestor) -> bool
                    {
                        int balance_factor = ancestor->BalanceFactor();
                        return std::abs(balance_factor) > 1;
                    },
                    /*inclusive*/ false);

                if (deepest_unbalanced)
                {
                    // Find the node in the direction of imbalance
                    // (making use of the binary search tree property: less --> left, greater --> right)
                    tree_node* in_direction_of_imbalance =
                        TComparator::LessThan(key, deepest_unbalanced->Key()) ?
                        deepest_unbalanced->Left() :
                        deepest_unbalanced->Right();

                    // Rebalance by performing an appropriate rotation
                    Rebalance(
                        deepest_unbalanced,
                        in_direction_of_imbalance);
                }
            }
            return node_inserted;
        }

        /**
         * @brief Removes a node having specified @p key from the tree.
         * Complexity: O(lgN), where N is the number of nodes in the tree.
         * @param key
         * @return true if a node with specified @p key was found in the tree, false otherwise.
         */
        bool remove(const K& key) override
        {
            tree_node* node_removed = this->find(key);
            if (node_removed)
            {
                // The node to replace the node being removed is either its in-order predecessor or its right child
                tree_node* in_order_predecessor = InOrderPredecessor<tree_node>(node_removed);
                tree_node* replacement = in_order_predecessor ? in_order_predecessor : node_removed->Right();

                tree_node* potential_imbalance_node = base::remove(node_removed, replacement);

                // If there might be an imbalance
                if (potential_imbalance_node)
                {
                    // Find the deepest node out of balance (search starts from 'potential_imbalance_node' inclusive)
                    tree_node* deepest_unbalanced =
                        FindAncestor<tree_node>(
                            potential_imbalance_node,
                            [](const tree_node* ancestor) -> bool
                            {
                                return std::abs(ancestor->BalanceFactor()) > 1;
                            },
                            /*inclusive*/ true);

                    if (deepest_unbalanced)
                    {
                        // Find the node in the direction of imbalance
                        // (making use of the binary search tree property: less --> left, greater --> right)
                        tree_node* in_direction_of_imbalance =
                            TComparator::LessThan(key, deepest_unbalanced->Key()) ?
                            deepest_unbalanced->Left() :
                            deepest_unbalanced->Right();

                        // Rebalance by performing an appropriate rotation
                        Rebalance(
                            deepest_unbalanced,
                            in_direction_of_imbalance);
                    }
                }

                return true;
            }
            else
                return false;
        }

    private:
        // Reestablishes the balance in the tree by moving nodes around.
        void Rebalance(
            tree_node* deepest_unbalanced,
            tree_node* in_direction_of_imbalance)
        {
            int deepest_balance_factor = deepest_unbalanced->BalanceFactor();
            int in_direction_balance_factor = in_direction_of_imbalance->BalanceFactor();

            if (deepest_balance_factor == 2 && in_direction_balance_factor == 1)
            {
                LeftRotate(in_direction_of_imbalance);
                this->refreshHeightUp(deepest_unbalanced);
            }
            else if (deepest_balance_factor == -2 && in_direction_balance_factor == -1)
            {
                RightRotate(in_direction_of_imbalance);
                this->refreshHeightUp(deepest_unbalanced);
            }
            else if (deepest_balance_factor == 2 && in_direction_balance_factor == -1)
            {
                RightRotate(in_direction_of_imbalance->Left());
                LeftRotate(in_direction_of_imbalance);
                this->refreshHeight(deepest_unbalanced);
                this->refreshHeightUp(in_direction_of_imbalance);
            }
            else if (deepest_balance_factor == -2 && in_direction_balance_factor == 1)
            {
                LeftRotate(in_direction_of_imbalance->Right());
                RightRotate(in_direction_of_imbalance);
                this->refreshHeight(deepest_unbalanced);
                this->refreshHeightUp(in_direction_of_imbalance);
            }
            else
            {
                // We shall never get here
                Requires::That(false, FUNCTION_INFO);
            }
        }

        // Shifts 'node' to the left to the place of its parent
        // which becomes 'node's left child.
        void LeftRotate(
            tree_node* node)
        {
            tree_node* parent = node->Parent();
            tree_node* grandparent = parent->Parent();
            tree_node* node_left = node->Left();

            if (grandparent)
            {
                if (TComparator::LessThan(node->Key(), grandparent->Key()))
                    grandparent->setLeft(node);
                else
                    grandparent->setRight(node);
            }
            else
                this->root = node;

            node->setParent(grandparent);

            parent->setRight(node_left);
            if(node_left)
                node_left->setParent(parent);

            node->setLeft(parent);
            parent->setParent(node);
        }

        // Shifts 'node' to the right to the place of its parent
        // which becomes 'node's right child.
        void RightRotate(
            tree_node* node)
        {
            tree_node* parent = node->Parent();
            tree_node* grandparent = parent->Parent();
            tree_node* node_right = node->Right();

            if (grandparent)
            {
                if (TComparator::LessThan(node->Key(), grandparent->Key()))
                    grandparent->setLeft(node);
                else
                    grandparent->setRight(node);
            }
            else
                this->root = node;

            node->setParent(grandparent);

            parent->setLeft(node_right);
            if(node_right)
                node_right->setParent(parent);

            node->setRight(parent);
            parent->setParent(node);
        }
    };
} // namespace cs
