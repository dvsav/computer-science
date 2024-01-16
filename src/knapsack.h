#pragma once

#include "requires.h"

#include <algorithm> // for std::max
#include <memory>    // for std::unique_ptr
#include <vector>    // for std::vector

namespace cs
{
    template <typename TValue>
    struct KnapsackItem
    {
        int weight;
        TValue value;

        KnapsackItem(
            size_t weight,
            TValue value) :
            weight(weight),
            value(value)
        {}
    };

    template <typename TValue>
    std::vector<KnapsackItem<TValue> > PackKnapsack(
        int maxWeight,
        const std::vector<KnapsackItem<TValue> >& items)
    {
        Requires::ArgumentPositive(maxWeight, NAMEOF(maxWeight), FUNCTION_INFO);

        const int n = items.size();
        std::unique_ptr<TValue> A(new TValue[maxWeight * n]);
        auto a = [&A, maxWeight](int i, int x) -> TValue
        {
            if (i < 0 || x < 1)
                return TValue(0);

            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        auto a_ref = [&A, maxWeight](int i, int x) -> TValue&
        {
            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        for (int i = 0; i < n; ++i)
        {
            for (int x = 1; x <= maxWeight; ++x)
            {
                if (x >= items[i].weight)
                {
                    a_ref(i, x) = std::max(
                        a(i - 1, x),
                        a(i - 1, x - items[i].weight) + items[i].value
                    );
                }
                else
                {
                    a_ref(i, x) = a(i - 1, x);
                }
            }
        }

        // Reconstruction
        std::vector<KnapsackItem<TValue> > knapsack;
        int x = maxWeight;
        for (int i = n - 1; i >= 0; --i)
        {
            if (a(i, x) == a(i - 1, x))
            {
                continue;
            }
            else if (a(i, x) == a(i - 1, x - items[i].weight) + items[i].value)
            {
                x -= items[i].weight;
                knapsack.push_back(items[i]);
            }
            else
            {
                Requires::That(false, FUNCTION_INFO);
            }
        }
        return knapsack;
    }
} // namespace cs
