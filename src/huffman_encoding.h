#pragma once

#include "requires.h"

#include <map>     // for std::map
#include <queue>   // for std::priority_queue
#include <stack>   // for std::stack
#include <utility> // for std::pair
#include <vector>  // for std::vector

namespace cs
{
    template <typename TSymbol, typename TFrequency>
    class HuffmanEncoding final
    {
    private:
        struct HuffmanSymbol
        {
            TSymbol symbol;
            TFrequency frequency;
            HuffmanSymbol* parent;
            HuffmanSymbol* left;
            HuffmanSymbol* right;
        };

        class SymbolComparator
        {
        public:
            bool operator() (
                const HuffmanSymbol* lhs,
                const HuffmanSymbol* rhs) const
            {
                return lhs->frequency > rhs->frequency;
            }
        };

    private:
        HuffmanSymbol* root;
        std::map<TSymbol, HuffmanSymbol*> leaves;

    public:
        HuffmanEncoding(
            std::vector<std::pair<TSymbol, TFrequency> >& alphabet) :
            root(nullptr),
            leaves()
        {
            std::priority_queue<HuffmanSymbol*, std::vector<HuffmanSymbol*>, SymbolComparator> minHeap;
            for (const std::pair<TSymbol, TFrequency>& symbol_freq : alphabet)
            {
                minHeap.push(
                    new HuffmanSymbol
                    {
                        /*symbol*/ symbol_freq.first,
                        /*frequency*/ symbol_freq.second,
                        /*parent*/ nullptr,
                        /*leftChild*/ nullptr,
                        /*rightChild*/ nullptr
                    });
            }

            while (true)
            {
                HuffmanSymbol* a = minHeap.top();
                minHeap.pop();

                HuffmanSymbol* b = minHeap.top();
                minHeap.pop();

                HuffmanSymbol* ab = new HuffmanSymbol
                {
                    /*symbol*/ TSymbol{},
                    /*frequency*/ a->frequency + b->frequency,
                    /*parent*/ nullptr,
                    /*leftChild*/ a,
                    /*rightChild*/ b
                };
                a->parent = ab;
                b->parent = ab;

                leaves.emplace(a->symbol, a);
                leaves.emplace(b->symbol, b);

                if (minHeap.empty())
                {
                    root = ab;
                    break;
                }

                minHeap.push(ab);
            }
        }

        ~HuffmanEncoding()
        {
            if (root)
            {
                std::stack<HuffmanSymbol*> st;
                st.push(root);
                while (!st.empty())
                {
                    HuffmanSymbol* ab = st.top();
                    st.pop();
                    if (ab->left) st.push(ab->left);
                    if (ab->right) st.push(ab->right);
                    delete ab;
                }
                root = nullptr;
            }
        }

    public:
        std::vector<bool> Encode(TSymbol symbol) const
        {
            HuffmanSymbol* current_node = leaves.at(symbol);
            std::vector<bool> encoded;
            while (current_node->parent)
            {
                encoded.insert(
                    /*where*/ encoded.begin(),
                    /*val*/ current_node == current_node->parent->right);

                current_node = current_node->parent;
            }
            return encoded;
        }

        TSymbol Decode(const std::vector<bool>& code)
        {
            HuffmanSymbol* current_node = root;
            for (bool right : code)
            {
                Requires::That(current_node, FUNCTION_INFO);
                current_node = right ? current_node->right : current_node->left;
            }
            Requires::That(current_node->left == nullptr && current_node->right == nullptr, FUNCTION_INFO);
            return current_node->symbol;
        }
    };
} // namespace cs
