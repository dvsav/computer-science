#pragma once

#include "utility.h"

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
    void PrintTree(std::ostream& os, TreeNode<T>* root, const std::string& prefix = "")
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

    private:
        tree_node* root;

    public:
        BinarySearchTree() :
            root(nullptr)
        {}

    public:
        const tree_node* Root() const { return root; }

        std::pair<tree_node*, bool> insert(const T& value)
        {
            if (!root)
                return (root = new tree_node(value));

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
    };
} // namespace cs
