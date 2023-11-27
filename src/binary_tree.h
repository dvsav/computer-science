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
     * @brief Class representing a node in a tree (e.g. binary search tree).
     * Has 'key' and 'value' fields. 'key' is supposed to be used for binary search.
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
     * @brief Deteles every node of specified tree.
     * @param root - tree root.
     * @tparam TTreeNode node type.
     */
    template <typename TTreeNode>
    void DeleteTree(
        TTreeNode* root)
    {
        PostOrderTraverse<TTreeNode>(
            root,
            [](TTreeNode* node) -> void
            {
                delete node;
            });
    }

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

    template <typename TTreeNode>
    TTreeNode* FindAncestor(
        TTreeNode* child,
        std::function<bool(const TTreeNode*)> predicate)
    {
        if (!child)
            return nullptr;

        TTreeNode* current_node = child;
        do
        {
            current_node = current_node->Parent();
        } while (current_node && !predicate(current_node));
        return current_node;
    }

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

    template <typename TTreeNode>
    int Height(const TTreeNode* node)
    {
        return std::max(
            node->Right() ? 1 + Height(node->Right()) : 0,
            node->Left() ? 1 + Height(node->Left()) : 0);
    }

    template <typename TTreeNode>
    int BalanceFactor(const TTreeNode* node)
    {
        return
            (node->Right() ? 1 + Height(node->Right()) : 0) -
            (node->Left() ? 1 + Height(node->Left()) : 0);
    }

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
        BinarySearchTree() :
            root(nullptr)
        {}

        BinarySearchTree(BinarySearchTree&& rvalue) :
            root(rvalue.root)
        {
            rvalue.root = nullptr;
        }

        BinarySearchTree(const BinarySearchTree&) = delete;

        virtual ~BinarySearchTree()
        {
            DeleteTree(root);
            root = nullptr;
        }

    public:
        BinarySearchTree& operator=(const BinarySearchTree&) = delete;
        
        tree_node* Root() { return root; }
        const tree_node* Root() const { return root; }

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

        V& operator[](const K& key)
        {
            tree_node* node = find(key);
            if (node)
                return node->Value();
            else
                return insert(key, V()).first->Value();
        }

        const V& operator[](const K& key) const
        {
            tree_node* node = find(key);
            if (node)
                return node->Value();
            else
                return insert(key, V()).first->Value();
        }

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
        // Returns potential imbalance node
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

    template <typename K, typename V, typename TComparator = DefaultComparator<K> >
    class AvlTree : public BinarySearchTree<K, V, TComparator>
    {
    public:
        using base = BinarySearchTree<K, V, TComparator>;
        using key_type = typename base::key_type;
        using value_type = typename base::value_type;
        using tree_node = typename base::tree_node;

    public:
        using base::BinarySearchTree;

    public:
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
                    });

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
                        std::abs(potential_imbalance_node->BalanceFactor()) > 1 ?
                        potential_imbalance_node :
                        FindAncestor<tree_node>(
                            potential_imbalance_node,
                            [](const tree_node* ancestor) -> bool
                            {
                                int balance_factor = ancestor->BalanceFactor();
                                return std::abs(balance_factor) > 1;
                            });

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
