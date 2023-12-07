#pragma once

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
        std::vector< std::list< std::pair<K, V> > >* buckets;

    public:
        HashMap() :
            buckets(nullptr)
        {}

        HashMap(HashMap&& rhs) :
            buckets(rhs.buckets)
        {
            rhs.buckets = nullptr;
        }

        ~HashMap()
        {
            if (buckets)
            {
                delete buckets;
                buckets = nullptr;
            }
        }

        HashMap(const HashMap&) = delete;
        HashMap& operator=(HashMap&) = delete;

    public:
        V& at(const K& key);
        const V& at(const K& key) const;

        V& operator[](const K& key);
        const V& operator[](const K& key) const;

        bool insert(const K& key, const V& value);

        bool erase(const K& key);
    };
} // namespace cs
