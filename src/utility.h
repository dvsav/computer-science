#pragma once

#include <string> // for std::string

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

    /**
     * @brief Returns true if files named @p fileNameA and @p fileNameB
     * are equal at textual level (compares line by line
     * ignoring possible differences in end-of-line symbols).
     * @param filePathA path to file A
     * @param filePathB path to file B
     * @return true if the contents of two files are equal
     * @return false if either of files is nonexistent or
     * if the files differ by at least one character
     * (not taking into account end-of-line symbols)
     */
    bool files_textually_equal(
        const std::string& filePathA,
        const std::string& filePathB);
}
