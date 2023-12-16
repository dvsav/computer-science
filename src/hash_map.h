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
    /**
     * @class HashMap
     * 
     * @brief Class representing a collection of key-value pairs where
     * a key can be found in O(1) time. Key-value pairs are stored in
     * an array; key-value pair index is calculated as 'hash(key) mod array_size'.
     * Collisions (when several key-value pairs fall to the same index) are
     * resolved using a technique called 'separate chaining' when colliding
     * key-value pairs are stored in lists called 'buckets'.
     * 
     * @tparam K key type
     * @tparam V value type
     * @tparam Hasher hash functor for key hashing
     */
    template <typename K, typename V, typename Hasher = std::hash<K> >
    class HashMap final
    {
    public:
        using key_type = K;
        using value_type = V;
        using bucket_type = std::list < std::pair<K, V> >;

    private:
        std::vector<bucket_type>* buckets;
        size_t nElements;

    public:
        /**
         * @brief Creates an empty hashmap.
         */
        HashMap() :
            buckets(new std::vector<bucket_type>(2, bucket_type{})),
            nElements(0)
        {}

        // Move constructor
        HashMap(HashMap&& rhs) :
            buckets(rhs.buckets),
            nElements(rhs.nElements)
        {
            rhs.buckets = nullptr;
            rhs.nElements = 0;
        }

        // Destructor
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
        /**
         * @brief Returns the number of key-value pairs in the hashmap.
         */
        size_t size() const { return nElements; }

        /**
         * @brief Returns the number of buckets in the hashmap.
         * 'Bucket' is a list stroring key-value pairs whose keys
         * hash to the same index (i.e. collide).
         */
        size_t buckets_number() const { return buckets->size(); }

        /**
         * @brief Returns the load of the hashmap, i.e. the number of
         * key-values pairs divided by the number of buckets.
         */
        float load() const { return nElements / (float)buckets->size(); }

        /**
         * @brief Returns a reference to a value of a key-value pair having
         * specified @p key. If such pair doesn't exists, an exception is thrown.
         * Complexity: O(1)
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& at(const K& key) { return find(key, nullptr)->second; }

        /**
         * @brief Returns a reference to a value of a key-value pair having
         * specified @p key. If such pair doesn't exists, an exception is thrown.
         * Complexity: O(1)
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        const V& at(const K& key) const { return find(key, nullptr)->second; }

        /**
         * @brief Returns a reference to a value of a key-value pair having specified @p key.
         * If such pair doesn't exists, it's inserted with default value.
         * Complexity: O(1)
         * @param key
         * @return reference to value corresponding to specified @p key.
         */
        V& operator[](const K& key)
        {
            std::pair<typename bucket_type::iterator, bool> result = insert(key, V{});
            return result.first->second;
        }

        /**
         * @brief Inserts a new key-value pair in the hashmap using specified @p key and @p value
         * and returns a pair (iterator-pointing-to-inserted-pair, true).
         * If such pair already exists, no action is taken and function returns
         * a pair (iterator-pointing-to-existing-pair, false).
         * Complexity: O(1)
         * @param key
         * @param value
         * @return a pair in which the first element is the iterator pointing to the key-value pair
         * and the second element is a boolean flag indicating whether the insertion was successful.
         * If a new key-value pair is created, fuction returns (pointer-to-new-pair, true).
         * If there is an existing pair which has specified @p key,
         * function returns (pointer-to-existing-pair, false).
         */
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
                resize(NextPrime(buckets->size()));

                // insert the new value
                size_t index = hash(key) % buckets->size();
                (*buckets)[index].emplace_front(std::make_pair(key, value));

                ++nElements;
                return std::make_pair((*buckets)[index].begin(), true);
            }
            else
            {
                pBucket->emplace_front(std::make_pair(key, value));

                ++nElements;
                return std::make_pair(pBucket->begin(), true);
            }
        }

        /**
         * @brief Removes a key-value pair having specified @p key from the hashmap.
         * Complexity: O(1) 
         * @param key
         * @return true if a key-value pair with specified @p key was
         * found in the hashmap and false otherwise.
         */
        bool erase(const K& key)
        {
            bucket_type* pBucket;
            typename bucket_type::iterator found = find(key, &pBucket);
            if (found != pBucket->end())
            {
                pBucket->erase(found);
                --nElements;

                if (load() < 0.5f)
                {
                    // load too low => decrease the number of buckets
                    size_t newSize = PreviousPrime(buckets->size());
                    if (newSize > 0)
                        resize(newSize);
                }

                return true;
            }
            return false;
        }

        /**
         * @brief Removes all key-value pairs from the hashmap.
         */
        void clear()
        {
            delete buckets;
            nElements = 0;
            buckets = new std::vector<bucket_type>(2, bucket_type{});
        }

        /**
         * @brief Returns true if a key-value pair having specified @p key
         * exists in the hashmap.
         * @param key
         * @return true if a key-value pair having specified @p key
         * exists in the hashmap.
         */
        bool contains(const K& key) const
        {
            const bucket_type* pBucket;
            typename bucket_type::const_iterator found = find(key, &pBucket);
            return (found != pBucket->cend());
        }

        /**
         * @brief Visits all key-value pairs in the hashmap by calling @p visitor
         * functor for each.
         * @param visitor - functor called for each key-value pair.
         */
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

        /**
         * @brief Visits all key-value pairs in the hashmap by calling @p visitor
         * functor for each.
         * @param visitor - functor called for each key-value pair.
         */
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

        /**
         * @brief Visits all bucket sizes by calling @p visitor for each.
         * @param visitor - functor called for each bucket size.
         */
        void visit_bucket_sizes(std::function<void(size_t)> visitor) const
        {
            for (const bucket_type& bucket : *buckets)
            {
                visitor(bucket.size());
            }
        }

    private:
        // Hashes a key
        static size_t hash(const K& key)
        {
            static Hasher hasher;
            return hasher(key);
        }

        // Searches for a key in the hashmap. Puts the bucket in which the key
        // is contained if exists (or should be contained if doesn't exist).
        // Returns an iterator pointing to key-value pair if it exists and bucket.end() otherwise.
        typename bucket_type::iterator find(
            const K& key,
            /*out*/ bucket_type** pBucket)
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

        // Searches for a key in the hashmap. Puts the bucket in which the key
        // is contained if exists (or should be contained if doesn't exist).
        // Returns an iterator pointing to key-value pair if it exists and bucket.end() otherwise.
        typename bucket_type::const_iterator find(
            const K& key,
            const bucket_type** pBucket) const
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

        // Allocates new collection of buckets, copies values from
        // old buckets to the new ones and deallocates old buckets.
        void resize(size_t newSize)
        {
            // allocate new buckets
            std::vector<bucket_type>* new_buckets = new std::vector<bucket_type>(
                /*count*/ newSize,
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

            // delete old buckets
            delete buckets;
            // update buckets
            buckets = new_buckets;
        }
    };
} // namespace cs
