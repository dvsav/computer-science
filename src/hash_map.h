#pragma once

#include "requires.h" // for Requires

#include <algorithm>  // for find_if
#include <cstddef>    // for size_t
#include <functional> // for std::hash
#include <list>       // for std::list
#include <vector>     // for std::vector
#include <utility>    // for std::pair

namespace cs
{
    template <typename K, typename V, typename Hasher = std::hash<K> >
    class HashMap
    {
    private:
        using bucket_type = std::list < std::pair<K, V> >;
        std::vector<bucket_type>* buckets;
        size_t nElements;

    public:
        HashMap() :
            buckets(nullptr),
            nElements(0)
        {}

        HashMap(HashMap&& rhs) :
            buckets(rhs.buckets),
            nElements(rhs.nElements)
        {
            rhs.buckets = nullptr;
            rhs.nElements = 0;
        }

        ~HashMap()
        {
            if (buckets)
            {
                delete buckets;
                buckets = nullptr;
                nElements = 0;
            }
        }

        HashMap(const HashMap&) = delete;
        HashMap& operator=(HashMap&) = delete;

    public:
        size_t size() const { return nElements; }

        V& at(const K& key) { return find(key, nullptr)->second; }
        const V& at(const K& key) const { return find(key, nullptr)->second; }

        V& operator[](const K& key);
        const V& operator[](const K& key) const;

        bool insert(const K& key, const V& value)
        {
            bucket_type* pBucket;
            bucket_type::iterator found = find(key, &pBucket);
            if (found != pBucket->end())
            {
                // key is already in the hash map
                return false;
            }

            float load = nElements / (float)buckets->size();
            if (load >= 1.0f)
            {
                // load too high => increase the number of buckets
                std::vector<bucket_type>* new_buckets = new std::vector<bucket_type>(
                    /*count*/ next_prime(buckets->size()),
                    /*value*/ bucket_type{});

                for (bucket_type& bucket : *buckets)
                {
                    for (const std::pair<K, V>& element : bucket)
                    {
                        size_t index = hash(/*key*/ element.first) % new_buckets->size();
                        (*new_buckets)[index].emplace_front(element);
                    }
                }

                delete buckets;
                buckets = new_buckets;
            }
            else
            {
                pBucket->emplace_front(std::make_pair(key, value));
            }

            return true;
        }

        bool erase(const K& key);

    private:
        static size_t hash(const K& key)
        {
            static Hasher hasher;
            return hasher(key);
        }

        bucket_type::iterator find(const K& key, typename bucket_type** pBucket)
        {
            size_t index = hash(key) % buckets->size();
            bucket_type& bucket = (*buckets)[index];
            if (pBucket)
                *pBucket = &bucket;
            return std::find_if(
                bucket.begin(), bucket.end(),
                /*pred*/
                [&key](const std::pair<K, V>& element) -> bool
                {
                    return element.first == key;
                });
        }

        bucket_type::const_iterator find(const K& key, typename const bucket_type** pBucket) const
        {
            size_t index = hash(key) % buckets->size();
            const bucket_type& bucket = (*buckets)[index];
            if (pBucket)
                *pBucket = &bucket;
            return std::find_if(
                bucket.cbegin(), bucket.cend(),
                /*pred*/
                [&key](const std::pair<K, V>& element) -> bool
                {
                    return element.first == key;
                });
        }
    };
} // namespace cs
