#pragma once

#include <queue>  // for std::priority_queue
#include <stack>  // for std::stack
#include <vector> // for std::vector

namespace cs
{
    template <typename TSymbol, typename TFrequency>
    class HuffmanEncoding final
    {
    private:
        class HuffmanSymbol
        {
            template <typename, typename>
            friend class HuffmanEncoding;

        private:
            TSymbol symbol;
            TFrequency frequency;
            HuffmanSymbol* leftChild;
            HuffmanSymbol* rightChild;

        private:
            HuffmanSymbol(
                bool rightOrLeft,
                TSymbol symbol,
                TFrequency frequency,
                HuffmanSymbol* leftChild = nullptr,
                HuffmanSymbol* rightChild = nullptr) :
                symbol(symbol),
                frequency(frequency),
                leftChild(leftChild),
                rightChild(rightChild)
            {}

            HuffmanSymbol(const HuffmanSymbol&) = delete;
            HuffmanSymbol& operator=(const HuffmanSymbol&) = delete;

        private:
            TSymbol Symbol() const { return symbol; }

            TFrequency Frequency() const { return frequency; }

            HuffmanSymbol* LeftChild() { return leftChild; }
            const HuffmanSymbol* LeftChild() const { return leftChild; }

            HuffmanSymbol* RightChild() { return rightChild; }
            const HuffmanSymbol* RightChild() const { return rightChild; }
        };

        class SymbolComparator
        {
        public:
            bool operator() (
                const HuffmanSymbol* lhs,
                const HuffmanSymbol* rhs)
            {
                return lhs->Frequency() > rhs->Frequency();
            }
        };

    private:
        HuffmanSymbol* root;

    private:
        HuffmanEncoding(
            std::priority_queue<HuffmanSymbol*, std::vector<HuffmanSymbol*>, SymbolComparator>& minHeap) :
            root(nullptr)
        {
            while (true)
            {
                HuffmanSymbol* a = minHeap.top();
                minHeap.pop();

                HuffmanSymbol* b = minHeap.top();
                minHeap.pop();

                HuffmanSymbol* ab = new HuffmanSymbol(
                    /*symbol*/ TSymbol{},
                    /*frequency*/ a->Frequency() + b->Frequency(),
                    /*leftChild*/ a,
                    /*rightChild*/ b);

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
                    if (ab->LeftChild()) st.push(ab->LeftChild());
                    if (ab->RightChild()) st.push(ab->RightChild());
                    delete ab;
                }
                root = nullptr;
            }
        }

    public:
        std::vector<bool> Encode(TSymbol symbol) const
        {
            //std::stack<bool> encoded;
            //HuffmanSymbol* current_node = root;
            //while (true)
            //{
            //    if (!current_node->LeftChild() && !current_node->RightChild())
            //    {
            //        if (current_node->Symbol() == symbol)
            //            return std::vector<bool>(encoded);
            //        else
            //            encoded.pop();

            //    if (current_node->LeftChild())
            //    encoded.push(root);


            //    if (TComparator::EqualTo(key, current_node->Key()))
            //        return current_node;
            //    else if (TComparator::LessThan(key, current_node->Key()))
            //        current_node = current_node->Left();
            //    else
            //        current_node = current_node->Right();
            //}
        }
    };
} // namespace cs
