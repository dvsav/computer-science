#pragma once

#include "prime.h"    // for NextPrime
#include "requires.h" // for Requires

#include <algorithm>  // for find_if
#include <cstddef>    // for size_t
#include <functional> // for std::hash, std::function
#include <list>       // for std::list
#include <vector>     // for std::vector
#include <utility>    // for std::pair

namespace cs
{
    template <typename K, typename V, typename Hasher = std::hash<K> >
    class HashMap
    {
    public:
        using key_type = K;
        using value_type = V;
        using bucket_type = std::list < std::pair<K, V> >;

    private:
        std::vector<bucket_type>* buckets;
        size_t nElements;

    public:
        HashMap() :
            buckets(new std::vector<bucket_type>(2, bucket_type{})),
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

        size_t buckets_number() const { return buckets->size(); }

        float load() const { return nElements / (float)buckets->size(); }

        V& at(const K& key) { return find(key, nullptr)->second; }

        const V& at(const K& key) const { return find(key, nullptr)->second; }

        V& operator[](const K& key)
        {
            std::pair<bucket_type::iterator, bool> result = insert(key, V{});
            return result.first->second;
        }

        std::pair<typename bucket_type::iterator, bool> insert(const K& key, const V& value)
        {
            bucket_type* pBucket;
            typename bucket_type::iterator found = find(key, &pBucket);
            if (found != pBucket->end())
            {
                // key is already in the hash map
                return std::make_pair(found, false);
            }

            if (load() >= 1.0f)
            {
                // load too high => increase the number of buckets
                std::vector<bucket_type>* new_buckets = new std::vector<bucket_type>(
                    /*count*/ NextPrime(buckets->size()),
                    /*value*/ bucket_type{});

                // copy elements from old buckets to new ones
                for (bucket_type& bucket : *buckets)
                {
                    for (const std::pair<K, V>& element : bucket)
                    {
                        size_t index = hash(/*key*/ element.first) % new_buckets->size();
                        (*new_buckets)[index].emplace_front(element);
                    }
                }
                // insert the new value
                size_t index = hash(key) % new_buckets->size();
                (*new_buckets)[index].emplace_front(std::make_pair(key, value));

                // delete old buckets
                delete buckets;
                // update buckets
                buckets = new_buckets;

                ++nElements;
                return std::make_pair((*new_buckets)[index].begin(), true);
            }
            else
            {
                pBucket->emplace_front(std::make_pair(key, value));

                ++nElements;
                return std::make_pair(pBucket->begin(), true);
            }
        }

        bool erase(const K& key)
        {
            bucket_type* pBucket;
            typename bucket_type::iterator found = find(key, &pBucket);
            if (found != pBucket->end())
            {
                pBucket->erase(found);
                --nElements;
                return true;
            }
            return false;
        }

        bool contains(const K& key) const
        {
            const bucket_type* pBucket;
            typename bucket_type::const_iterator found = find(key, &pBucket);
            return (found != pBucket->cend());
        }

        void visit(std::function<void(const K&, const V&)> visitor) const
        {
            for (const bucket_type& bucket : *buckets)
            {
                for (const std::pair<K, V>& element : bucket)
                {
                    visitor(element.first, element.second);
                }
            }
        }

        void visit(std::function<void(const K&, V&)> visitor)
        {
            for (bucket_type& bucket : *buckets)
            {
                for (std::pair<K, V>& element : bucket)
                {
                    visitor(element.first, element.second);
                }
            }
        }

        void visit_bucket_sizes(std::function<void(size_t)> visitor) const
        {
            for (const bucket_type& bucket : *buckets)
            {
                visitor(bucket.size());
            }
        }

    private:
        static size_t hash(const K& key)
        {
            static Hasher hasher;
            return hasher(key);
        }

        typename bucket_type::iterator find(
            const K& key,
            typename bucket_type** pBucket)
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

        typename bucket_type::const_iterator find(
            const K& key,
            typename const bucket_type** pBucket) const
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
