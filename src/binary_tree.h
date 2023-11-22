#pragma once

#include <functional> // for std::function
#include <iostream>   // for std::ostream
#include <stack>      // for std::stack
#include <string>     // for std::string

namespace cs
{
    template <typename T>
    class TreeNode
    {
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
            TreeNode* left,
            TreeNode* right) :
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
        TreeNode* setLeft(TreeNode* node)
        {
            if (left)
                left->setParent(nullptr);

            left = node;

            if (node)
                left->setParent(this);

            return node;
        }

        const TreeNode* Right() const { return right; }
        TreeNode* Right() { return right; }
        TreeNode* setRight(TreeNode* node)
        {
            if (right)
                right->setParent(nullptr);

            right = node;

            if (node)
                right->setParent(this);

            return node;
        }

        const TreeNode* Parent() const { return parent; }
        TreeNode* Parent() { return parent; }
        TreeNode* setParent(TreeNode* node)
        {
            parent = node;
            return node;
        }

        const T& Value() const { return value; }
        T& Value() { return value; }
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
} // namespace cs
