#pragma once

#include <algorithm> // for std::max
#include <memory>    // for std::unique_ptr
#include <vector>    // for std::vector

namespace cs
{
    template <typename TValue>
    struct KnapsackItem
    {
        size_t weight;
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
        size_t maxWeight,
        const std::vector<KnapsackItem<TValue> >& items)
    {
        const size_t n = items.size();
        std::unique_ptr<TValue> A(new TValue[maxWeight * n]);
        auto a = [&A, maxWeight](size_t i, size_t x) -> TValue
        {
            if (i < 0 || x < 1)
                return TValue(0);

            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        auto a_ref = [&A, maxWeight](size_t i, size_t x) -> TValue&
        {
            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        for (size_t i = 0; i < items.size(); ++i)
        {
            for (size_t x = 1; x <= maxWeight; ++x)
            {
                a_ref(i, x) = std::max(
                    a(i - 1, x),
                    a(i - 1, x - items[i].weight) + items[i].value);
            }
        }

        // Reconstruction
        std::vector<KnapsackItem<TValue> > knapsack;
        size_t x = maxWeight;
        for (size_t i = n - 1; i > 0; --i)
        {
            if (a(i, x) == a(i - 1, x - items[i].weight) + items[i].value)
            {
                x -= items[i].weight;
                knapsack.push_back(items[i]);
            }
        }
        return knapsack;
    }
} // namespace cs
