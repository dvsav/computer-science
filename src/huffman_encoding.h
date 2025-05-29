#pragma once

#include "requires.h"

#include <map>     // for std::map
#include <queue>   // for std::priority_queue
#include <stack>   // for std::stack
#include <utility> // for std::pair, std::move
#include <vector>  // for std::vector

namespace cs
{
    /**
     * @class HuffmanEncoding
     * @brief Class providing Huffman encoding algorithm (so called prefix-free encoding).
     * Allows for encoding symbols of some alphabet. Each symbol has certain
     * associated frequency (what the probability is of encountering that symbol in a text).
     * A symbol is encoded to a set of bits. Length of this set of bits
     * depends on the frequency of the symbol - the more frequent the symbol, the shorter
     * the length of its encoding (set of bits).
     * @tparam TSymbol symbol type.
     * @tparam TFrequency symbol frequency (probability) type.
     */
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
        /**
         * @brief Creates a new Huffman encoding from a specified @p alphabet.
         * @param alphabet
         */
        HuffmanEncoding(
            std::vector<std::pair<TSymbol, TFrequency> >& alphabet) :
            root(nullptr),
            leaves()
        {
            // Algorithm:
            // Create a binary tree structure with the symbols of the alphabet
            // stored in the leaves of the tree. Binary encoding of a particular
            // symbol is obtained by traversing from the root of the tree to the
            // leaf storing the symbol where each edge denoted either 0 (if it
            // leads to a left child) or 1 (if it leads to a right child).
            // The less frequent symbols are stored in deeper leaves (therefore
            // they have longer binary encodings) while more frequent symbols are
            // stored closer to the root of the tree (therefore they have shorter
            // binary encodings).

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

        // Move constructor.
        HuffmanEncoding(HuffmanEncoding&& rval) :
            root(rval.root),
            leaves(std::move(rval.leaves))
        {
            rval.root = nullptr;
        }

        HuffmanEncoding(const HuffmanEncoding&) = delete;
        HuffmanEncoding& operator=(const HuffmanEncoding&) = delete;

    public:
        /**
         * @brief Returns @p symbol encoding as a vector of boolean values
         * which play a role of bits in binary encoding.
         * @param symbol - symbol to be encoded.
         * @return symbol's binary encoding.
         */
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

        /**
         * @brief Decodes some binary encoding represented as a vector of boolean
         * values to a symbol.
         * @param code - symbol's binary encoding.
         * @return decoded symbol.
         */
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
