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

    /**
     * @class BloomFilter.
     * @brief Class representing a set of values (a hashset). Supports 'insert' and 'contains' operations.
     * When inserted, each value leaves a 'bit footprint' - certain bits (depending on the value) set to 1.
     * Which bits are set to 1 is determined through 'NHashFunctions' hash function. Each function hashes the value
     * to the index of a bit which is set to 1. 'contains(value)' operation returns positive answer if certain bits
     * (depending on the value - see above) are set to 1. False positives are possible.
     * @tparam T value type.
     * @tparam NBits number of bits in the bit set.
     * @tparam NHashFunctions number of hash functions.
     * @tparam Hasher functor type which has operator() accepting two parameters: (size_t seed, const T& v)
     * and hashes 'v' (hash function depends on the 'seed').
     */
    template <typename T, unsigned int NBits, unsigned int NHashFunctions, typename Hasher = SeededHash<T> >
    class BloomFilter
    {
    private:
        std::bitset<NBits> bitArray;

    public:
        using value_type = T;

    public:
        /**
         * @brief Creates a bloom filter.
         */
        BloomFilter() :
            bitArray(0)
        {}

        BloomFilter(const BloomFilter&) = default;

    public:
        BloomFilter& operator=(const BloomFilter&) = default;

        /**
         * @brief Inserts a @p value to the set.
         * @param value
         */
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

        /**
         * @brief Returns true if @p value is in the set, but may also
         * return false positive.
         * @param value
         * @return true if @p value is in the set.
         */
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

        /**
         * @brief Prints the bit set to @p os output stream.
         * @param os - output stream.
         */
        void print(std::ostream& os) const
        {
            os << bitArray;
        }
    };
} // namespace cs
