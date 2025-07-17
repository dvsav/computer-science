#pragma once

#include "requires.h"

#include <algorithm> // for std::max
#include <memory>    // for std::unique_ptr
#include <vector>    // for std::vector

namespace cs
{
    /**
     * @class KnapsackItem
     * @brief Represents an item that can be put into
     * a knapsack (backpack). Characterized by weight
     * (integer variable) and value.
     * @tparam TValue value type.
     */
    template <typename TValue>
    struct KnapsackItem
    {
        int weight;
        TValue value;

        KnapsackItem(
            int weight,
            TValue value) :
            weight(weight),
            value(value)
        {
            Requires::ArgumentPositive(weight, NAMEOF(weight), FUNCTION_INFO);
        }
    };

    /**
     * @brief Solves the so called 'knapsack problem'.
     * The goal of this algorithm is to select items from a
     * specified set @p items which together make maximal
     * possible value and whose total weight doesn't exceed
     * @p maxWeight. Complexity: O(items.size() * maxWeight)
     * @param maxWeight - maximum weight that the knapsack can carry.
     * @param items - set of items from which we need to select.
     * @return selected items that together make maximal possible
     * value while not exceeding the maximum weight of the knapsack.
     */
    template <typename TValue>
    std::vector<KnapsackItem<TValue> > PackKnapsack(
        int maxWeight,
        const std::vector<KnapsackItem<TValue> >& items)
    {
        Requires::ArgumentPositive(maxWeight, NAMEOF(maxWeight), FUNCTION_INFO);

        // Knapsack problem's solution is an example of dynamic programming concept.
        // We have n items indexed from i=0 to i=n-1.
        // The full problem is reduced to the following two subproblems:
        // 1. If we take item i=n-1, solve the problem for n-1 items (i=0 to i=n-2) and max weight = maxWeight - (n-1)th item's weight.
        // 2. If we don't take item i=n-1, solve the problem for n-1 items (i=0 to i=n-2) and max weight = maxWeight.
        // From the two subproblems above pick the one giving the most total value of items.

        // We allocate an 2D array A with n rows and maxWeight columns.
        // A[i, x] where (i=0...n-1 and x=1...maxWeight) is the solution (total value)
        // of a subproblem for items={item[0]...item[i]} and max weight=x.
        const int n = static_cast<int>(items.size());
        std::unique_ptr<TValue[]> A(new TValue[maxWeight * n]);

        // Function that returns the A[i, x] element of array A (by value).
        auto a = [&A, maxWeight](int i, int x) -> TValue
        {
            if (i < 0 || x < 1)
                return TValue(0);

            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        // Function that returns the A[i, x] element of array A (by reference).
        auto a_ref = [&A, maxWeight](int i, int x) -> TValue&
        {
            // i = [0...n)
            // x = [1...maxWeight]
            return A.get()[i * maxWeight + x - 1];
        };

        // Solve all the subproblems from smallest to the largest.
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

        // Now we have the filled array A of subproblem solutions,
        // so we know the maximal value, but we don't know the set of
        // items that led to that value. So we need to do another pass
        // (called "reconstruction") from the full problem value A[n-1, maxWeight]
        // to the smallest subproblem A[0, ...].
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
