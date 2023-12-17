#pragma once

#include "utility.h"

#include <bitset>
#include <ostream>
#include <random>

namespace cs
{
    template <typename T>
    struct SeededHash
    {
        size_t operator()(std::size_t seed, const T& v)
        {
            return hash_combine<T>(seed, v);
        }
    };

    template <typename T, unsigned int NBits, unsigned int NHashFunctions, typename Hasher = SeededHash<T> >
    class BloomFilter
    {
    private:
        std::bitset<NBits> bitArray;

    public:
        BloomFilter() :
            bitArray(0)
        {}

        BloomFilter(const BloomFilter&) = default;

    public:
        BloomFilter& operator=(const BloomFilter&) = default;

        void insert(const T& value)
        {
            std::default_random_engine prng;
            std::uniform_int_distribution<int> uniform_dist(0, NBits-1);
            for (unsigned int i = 1; i <= NHashFunctions; ++i)
            {
                Hasher hasher;
                size_t nBit = hasher(uniform_dist(prng), value) % NBits;
                bitArray[nBit] = true;
            }
        }

        bool contains(const T& value) const
        {
            std::default_random_engine prng;
            std::uniform_int_distribution<int> uniform_dist(0, NBits - 1);
            for (unsigned int i = 1; i <= NHashFunctions; ++i)
            {
                Hasher hasher;
                size_t nBit = hasher(uniform_dist(prng), value) % NBits;
                if (!bitArray[nBit])
                    return false;
            }
            return true;
        }

        void print(std::ostream& os) const
        {
            os << bitArray;
        }
    };
} // namespace cs
