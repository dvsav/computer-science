#pragma once

namespace cs
{
    template <typename T>
    class DefaultComparator
    {
    public:
        static bool LessThan(const T& a, const T& b) { return a < b; }
        static bool EqualTo(const T& a, const T& b) { return a == b; }
        static bool LessThanOrEqualTo(const T& a, const T& b) { return a <= b; }
        static bool GreaterThan(const T& a, const T& b) { return a > b; }
        static bool GreaterThanOrEqualTo(const T& a, const T& b) { return a >= b; }
    };

    template <typename T>
    class ReverseComparator
    {
    public:
        static bool LessThan(const T& a, const T& b) { return a > b; }
        static bool EqualTo(const T& a, const T& b) { return a == b; }
        static bool LessThanOrEqualTo(const T& a, const T& b) { return a >= b; }
        static bool GreaterThan(const T& a, const T& b) { return a < b; }
        static bool GreaterThanOrEqualTo(const T& a, const T& b) { return a <= b; }
    };
}
