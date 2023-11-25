#pragma once

#include "utility.h"

#include <algorithm>   // for std::max
#include <functional>  // for std::function
#include <iostream>    // for std::ostream
#include <stack>       // for std::stack
#include <string>      // for std::string
#include <type_traits> // for std::is_same
#include <utility>     // for std:::pair

namespace cs
{
    template <typename T>
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
        T value;

    public:
        using value_type = T;

    public:
        TreeNode(
            const T& value,
            TreeNode* left = nullptr,
            TreeNode* right = nullptr) :
            parent(nullptr),
            left(left),
            right(right),
            value(value)
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

        const T& Value() const { return value; }
        T& Value() { return value; }

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

    template <typename T>
    void PreOrderTraverse(
        TreeNode<T>* root,
        std::function<void(TreeNode<T>*)> visitor)
    {
        if (root)
        {
            visitor(root);
            PreOrderTraverse<T>(root->Left(), visitor);
            PreOrderTraverse<T>(root->Right(), visitor);
        }
    }

    template <typename T>
    void InOrderTraverse(
        TreeNode<T>* root,
        std::function<void(TreeNode<T>*)> visitor)
    {
        if (root)
        {
            InOrderTraverse<T>(root->Left(), visitor);
            visitor(root);
            InOrderTraverse<T>(root->Right(), visitor);
        }
    }

    template <typename T>
    void PostOrderTraverse(
        TreeNode<T>* root,
        std::function<void(TreeNode<T>*)> visitor)
    {
        if (root)
        {
            PostOrderTraverse<T>(root->Left(), visitor);
            PostOrderTraverse<T>(root->Right(), visitor);
            visitor(root);
        }
    }

    template <typename T>
    void DeleteTree(
        TreeNode<T>* root)
    {
        PostOrderTraverse<T>(
            root,
            [](TreeNode<T>* node) -> void
            {
                delete node;
            });
    }

    template <typename T>
    void LevelOrderTraverse(
        TreeNode<T>* root,
        std::function<void(TreeNode<T>*)> visitor)
    {
        using tree_node = TreeNode<T>;

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

    template <typename T, bool IsRoot = true, bool IsLeft = false>
    void PrintTree(std::ostream& os, const TreeNode<T>* root, const std::string& prefix = "")
    {
        os << prefix;
        if (!IsRoot)
            os << (IsLeft ? "|--" : "`--");

        if (!root)
        {
            os << std::endl;
            return;
        }

        os << root->Value() << std::endl;

        if (!root->Left() && !root->Right())
            return;

        PrintTree<T, false, true>(os, root->Left(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
        PrintTree<T, false, false>(os, root->Right(), prefix + std::string(IsRoot ? "" : IsLeft ? "|  " : "   "));
    }

    template <typename T>
    TreeNode<T>* InORderPredecessor(TreeNode<T>* root)
    {
        using tree_node = TreeNode<T>;

        if (!root)
            return nullptr;

        tree_node* predecessor = root->Left();
        if (!predecessor)
            return nullptr;

        while (predecessor->Right())
            predecessor = predecessor->Right();

        return predecessor;
    }

    template <typename T>
    int Height(TreeNode<T>* root)
    {
        if (!root)
            return 0;

        return std::max(
            root->Left() ? 1 + Height(root->Left()) : 0,
            root->Right() ? 1 + Height(root->Right()) : 0);
    }

    template <typename T>
    int BalanceFactor(const TreeNode<T>* node)
    {
        return Height(node->Right()) - Height(node->Left());
    }

    template <typename T, typename TComparator = DefaultComparator<T> >
    class BinarySearchTree
    {
        // Check for the existence of a member function "LessThan" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::LessThan),
                         bool (*)(const T&, const T&)>::value,
            "LessThan function is missing in TComparator");

        // Check for the existence of a member function "EqualTo" in TComparator type
        static_assert(
            std::is_same<decltype(&TComparator::EqualTo),
                         bool (*)(const T&, const T&)>::value,
            "EqualTo function is missing in TComparator");

    public:
        using value_type = T;
        using tree_node = TreeNode<T>;

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

        tree_node* find(const T& value)
        {
            tree_node* current_node = root;
            while (true)
            {
                if (!current_node)
                    return nullptr;

                if (TComparator::EqualTo(value, current_node->Value()))
                    return current_node;
                else if (TComparator::LessThan(value, current_node->Value()))
                    current_node = current_node->Left();
                else
                    current_node = current_node->Right();
            }
        }

        virtual std::pair<tree_node*, bool> insert(const T& value)
        {
            if (!root)
            {
                root = new tree_node(value);
                return std::make_pair(root, true);
            }

            tree_node* current_node = root;
            while (true)
            {
                if (TComparator::EqualTo(value, current_node->Value()))
                {
                    return std::make_pair(current_node, false);
                }
                else if (TComparator::LessThan(value, current_node->Value()))
                {
                    if (current_node->Left())
                        current_node = current_node->Left();
                    else
                        return std::make_pair(current_node->setLeft(new tree_node(value)), true);
                }
                else
                {
                    if (current_node->Right())
                        current_node = current_node->Right();
                    else
                        return std::make_pair(current_node->setRight(new tree_node(value)), true);
                }
            }
        }

        virtual bool remove(const T& value)
        {
            tree_node* node_removed = find(value);
            if (!node_removed)
                return false;

            tree_node* left = node_removed->Left();
            tree_node* right = node_removed->Right();
            tree_node* parent = node_removed->Parent();

            // the node to replace the node being removed is either its in-order predecessor or its right child
            tree_node* in_order_predecessor = InORderPredecessor<T>(node_removed);
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

    template <typename T, typename TComparator = DefaultComparator<T> >
    class AvlTree : public BinarySearchTree<T, typename TComparator>
    {
    public:
        using base = BinarySearchTree<T, typename TComparator>;
        using value_type = base::value_type;
        using tree_node = base::tree_node;

    public:
        using base::BinarySearchTree;

    public:
        std::pair<tree_node*, bool> insert(const T& value) override
        {
            auto node_inserted = base::insert(value);
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
