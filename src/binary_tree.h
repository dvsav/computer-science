#pragma once

#include "utility.h"   // for DefaultComparator

#include <algorithm>   // for std::max
#include <functional>  // for std::function
#include <iostream>    // for std::ostream
#include <stack>       // for std::stack
#include <string>      // for std::string
#include <type_traits> // for std::is_same
#include <utility>     // for std:::pair

namespace cs
{
    template <typename K>
    class TreeNode
    {
        template <typename, typename>
        friend class BinarySearchTree;

        template <typename, typename>
        friend class AvlTree;

    private:
        TreeNode* parent;
        TreeNode* left;
        TreeNode* right;
        K key;

    public:
        using key_type = K;

    public:
        TreeNode(
            const K& key,
            TreeNode* left = nullptr,
            TreeNode* right = nullptr) :
            parent(nullptr),
            left(left),
            right(right),
            key(key)
        {
            if (left)
                left->setParent(this);

            if (right)
                right->setParent(this);
        }

        TreeNode(const TreeNode&) = delete;
        TreeNode& operator=(const TreeNode&) = delete;

    public:
        const TreeNode* Left() const { return left; }
        TreeNode* Left() { return left; }

        const TreeNode* Right() const { return right; }
        TreeNode* Right() { return right; }

        const TreeNode* Parent() const { return parent; }
        TreeNode* Parent() { return parent; }

        const K& Key() const { return key; }
        K& Key() { return key; }

    private:
        TreeNode* setLeft(TreeNode* node)
        {
            if (left)
                left->setParent(nullptr);

            left = node;

            if (node)
                left->setParent(this);

            return node;
        }

        TreeNode* setRight(TreeNode* node)
        {
            if (right)
                right->setParent(nullptr);

            right = node;

            if (node)
                right->setParent(this);

            return node;
        }

        TreeNode* setParent(TreeNode* node)
        {
            parent = node;
            return node;
        }
    };

    template <typename K>
    void PreOrderTraverse(
        TreeNode<K>* root,
        std::function<void(TreeNode<K>*)> visitor)
    {
        if (root)
        {
            visitor(root);
            PreOrderTraverse<K>(root->Left(), visitor);
            PreOrderTraverse<K>(root->Right(), visitor);
        }
    }

    template <typename K>
    void InOrderTraverse(
        TreeNode<K>* root,
        std::function<void(TreeNode<K>*)> visitor)
    {
        if (root)
        {
            InOrderTraverse<K>(root->Left(), visitor);
            visitor(root);
            InOrderTraverse<K>(root->Right(), visitor);
        }
    }

    template <typename K>
    void PostOrderTraverse(
        TreeNode<K>* root,
        std::function<void(TreeNode<K>*)> visitor)
    {
        if (root)
        {
            PostOrderTraverse<K>(root->Left(), visitor);
            PostOrderTraverse<K>(root->Right(), visitor);
            visitor(root);
        }
    }

    template <typename K>
    void DeleteTree(
        TreeNode<K>* root)
    {
        PostOrderTraverse<K>(
            root,
            [](TreeNode<K>* node) -> void
            {
                delete node;
            });
    }

    template <typename K>
    void LevelOrderTraverse(
        TreeNode<K>* root,
        std::function<void(TreeNode<K>*)> visitor)
    {
        using tree_node = TreeNode<K>;

        if (!root)
            return;

        std::stack<tree_node*> level;
        level.push(root);

        while (!level.empty())
        {
            tree_node* node = level.top();
            level.pop();

            if (node->Left())
                level.push(node->Left());
            if (node->Right())
                level.push(node->Right());

            // visit after we are done with node's children
            visitor(node);
        }
    }

    template <typename K, bool IsRoot = true, bool IsLeft = false>
    void PrintTree(std::ostream& os, const TreeNode<K>* root, const std::string& prefix = "")
    {
        os << prefix;
        if (!IsRoot)
            os << (IsLeft ? "|--" : "`--");

        if (!root)
        {
            os << std::endl;
            return;
        }

        os << root->Key() << std::endl;

        if (!root->Left() && !root->Right())
            return;

        PrintTree<K, false, true>(os, root->Left(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
        PrintTree<K, false, false>(os, root->Right(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
    }

    template <typename K>
    TreeNode<K>* InORderPredecessor(TreeNode<K>* root)
    {
        using tree_node = TreeNode<K>;

        if (!root)
            return nullptr;

        tree_node* predecessor = root->Left();
        if (!predecessor)
            return nullptr;

        while (predecessor->Right())
            predecessor = predecessor->Right();

        return predecessor;
    }

    template <typename K>
    int Height(TreeNode<K>* root)
    {
        if (!root)
            return 0;

        return std::max(
            root->Left() ? 1 + Height(root->Left()) : 0,
            root->Right() ? 1 + Height(root->Right()) : 0);
    }

    template <typename K>
    int BalanceFactor(const TreeNode<K>* node)
    {
        return Height(node->Right()) - Height(node->Left());
    }

    template <typename K, typename TComparator = DefaultComparator<K> >
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
        using tree_node = TreeNode<K>;

    protected:
        tree_node* root;

    public:
        BinarySearchTree() :
            root(nullptr)
        {}

        virtual ~BinarySearchTree()
        {
            DeleteTree(root);
            root = nullptr;
        }

    public:
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

        virtual std::pair<tree_node*, bool> insert(const K& key)
        {
            if (!root)
            {
                root = new tree_node(key);
                return std::make_pair(root, true);
            }

            tree_node* current_node = root;
            while (true)
            {
                if (TComparator::EqualTo(key, current_node->Key()))
                {
                    return std::make_pair(current_node, false);
                }
                else if (TComparator::LessThan(key, current_node->Key()))
                {
                    if (current_node->Left())
                        current_node = current_node->Left();
                    else
                        return std::make_pair(current_node->setLeft(new tree_node(key)), true);
                }
                else
                {
                    if (current_node->Right())
                        current_node = current_node->Right();
                    else
                        return std::make_pair(current_node->setRight(new tree_node(key)), true);
                }
            }
        }

        virtual bool remove(const K& key)
        {
            tree_node* node_removed = find(key);
            if (!node_removed)
                return false;

            tree_node* left = node_removed->Left();
            tree_node* right = node_removed->Right();
            tree_node* parent = node_removed->Parent();

            // the node to replace the node being removed is either its in-order predecessor or its right child
            tree_node* in_order_predecessor = InORderPredecessor<K>(node_removed);
            tree_node* replacement = in_order_predecessor ? in_order_predecessor : node_removed->Right();

            if (replacement)
            {
                // update replacement's parent's child pointer
                replacement->Parent()->setRight(nullptr);

                // update replacement's children pointers
                if (replacement != left)
                    replacement->setLeft(left);
                if (replacement != right)
                    replacement->setRight(right);
            }

            // update parent's child pointer
            if (parent)
            {
                if (parent->Left() == node_removed)
                    parent->setLeft(replacement);
                else if (parent->Right() == node_removed)
                    parent->setRight(replacement);
            }
            else
                root = replacement;

            delete node_removed;
            return true;
        }
    };

    template <typename K, typename TComparator = DefaultComparator<K> >
    class AvlTree : public BinarySearchTree<K, TComparator>
    {
    public:
        using base = BinarySearchTree<K, TComparator>;
        using key_type = typename base::key_type;
        using tree_node = typename base::tree_node;

    public:
        using base::BinarySearchTree;

    public:
        std::pair<tree_node*, bool> insert(const K& key) override
        {
            auto node_inserted = base::insert(key);
            if (node_inserted.second)
            {
                tree_node* new_node = node_inserted.first;
                // find the deepest node out of balance
                // find the node in the direction of imbalance
                // perform the appropriate rotation
            }
            else
                return node_inserted;
        }
    };
} // namespace cs
