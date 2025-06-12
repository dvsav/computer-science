#pragma once

#include <cctype>   // for std::isspace
#include <iostream> // for std::ostream, std::istream
#include <sstream>  // for std::stringstream
#include <string>   // for std::string
#include <vector>   // for std::vector

template <int N, int D>
struct CeilDivision
{
    static constexpr int value = (N + D - 1) / D;
};

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
 * @return true if the contents of two files are equal and
 * false if either of files is nonexistent or if the files differ
 * by at least one character
 * (not taking into account end-of-line symbols)
 */
bool files_textually_equal(
    const std::string& filePathA,
    const std::string& filePathB);

/**
 * @brief Reads and discards all whitespace characters until
 * it encounters a non-whitespace one.
 * @param is - input stream
 */
inline void skip_whitespace(std::istream& is)
{
    int ch{};
    // Read characters until a non-whitespace character is encountered
    while ((ch = is.peek()) != EOF && std::isspace(ch))
        is.get(); // Discard the whitespace character
}

/**
 * @brief Removes leading spaces from the given string.
 *
 * This function modifies the input string by erasing all leading space characters.
 * If the string consists entirely of spaces, it will be cleared and become empty.
 *
 * @param str Reference to the string to be trimmed.
 */
inline void trim_leading_spaces(std::string& str)
{
    if (size_t start = str.find_first_not_of(' ') != std::string::npos)
        str.erase(0, start);
    else // The string is all spaces
        str.clear();
}

/**
 * @brief Reads and discards a single line beginning with specified string @p comment_begins_with.
 * @param is - input stream
 * @param comment_begins_with - the string that begins the comment
 * @return true the input line was a comment and false otherwise
 */
bool skip_comment(
    std::istream& is,
    const std::string& comment_begins_with);

/**
 * @brief This operator reads space-separated elements of type T from
 * the input stream (std::istream) and pushes them into a std::vector<T>.
 * The reading stops when a newline character is encountered.
 */
template<typename T>
inline std::istream& operator>>(
    std::istream& is,
    std::vector<T>& vec)
{
    // Clear the vector to ensure it's empty before reading
    vec.clear();

    // Skip all whitespace characters and check that the first
    // non-whitespace one is a digit
    skip_whitespace(is);
    if (!std::isdigit(is.peek()))
    {
        is.clear(std::ios_base::failbit);
        return is;
    }

    // Read the entire line until newline
    std::string line;
    std::getline(is, line);
    if (!is)
        return is;

    std::istringstream iss(line);
    T element;

    // Read space-separated elements from the line
    while (iss >> element)
        vec.push_back(element);

    // If iss failed, put is to failed state as well
    if (!iss.eof() && iss.fail())
        is.clear(std::ios_base::failbit);

    return is;
}

/**
 * @brief Prints out a collection of elements specified by a 
 * pair of iterators @p begin and @p end to a specified output stream @p os.
 * The elements are separated by a specified @p delimeter.
 */
template <typename TIterator>
inline void print(
    std::ostream& os,
    TIterator begin,
    TIterator end,
    const std::string& delimeter)
{
    for (auto i = begin; i != end; i++)
        os << *i << delimeter;
}

/**
 * @brief Prints out a collection of elements specified by a 
 * pair of iterators @p begin and @p end to the standard output stream.
 * The elements are separated by a specified @p delimeter.
 */
template <typename TIterator>
inline void print(
    TIterator begin,
    TIterator end,
    const std::string& delimeter)
{
    print(std::cout, begin, end, delimeter);
}

// From boost
template <class T>
inline std::size_t hash_combine(std::size_t seed, const T& v)
{
    std::hash<T> hasher;
    return seed ^ (hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

/**
 * @brief Shifts an integral value left by a specified offset.
 *
 * This function shifts the given value @p val left by @p offset bits.
 * - If `offset` is positive, the value is shifted left.
 * - If `offset` is negative, the value is shifted right.
 * - If `offset` is zero, the value is returned unchanged.
 *
 * @tparam T An integral type.
 * @param val The value to shift.
 * @param offset The number of bits to shift. Positive for left shift, negative for right shift.
 * @return The shifted value.
 *
 * @note A static assertion ensures that T is an integral type.
 */
template <class T>
inline T shiftLeft(T val, int offset)
{
    static_assert(std::is_integral<T>::value, "T must be an integral type");

    if (offset > 0)
        return static_cast<T>(val << offset);
    else if (offset < 0)
        return static_cast<T>(val >> (-offset));
    else
        return val;
}

/**
 * @brief Shifts an integral value right by a specified offset.
 *
 * This function shifts the given value @p val right by @p offset bits.
 * - If `offset` is positive, the value is shifted right.
 * - If `offset` is negative, the value is shifted left.
 * - If `offset` is zero, the value is returned unchanged.
 *
 * @tparam T An integral type.
 * @param val The value to shift.
 * @param offset The number of bits to shift. Positive for right shift, negative for left shift.
 * @return The shifted value.
 *
 * @note A static assertion ensures that T is an integral type.
 */
template <class T>
inline T shiftRight(T val, int offset)
{
    static_assert(std::is_integral<T>::value, "T must be an integral type");

    if (offset > 0)
        return static_cast<T>(val >> offset);
    else if (offset < 0)
        return static_cast<T>(val << (-offset));
    else
        return val;
}

/**
 * @brief Returns the index of the highest nonzero bit in an integral value.
 *
 * This function computes the position of the most significant bit set to 1 in the given
 * integral @p value. If the value is 0, the function returns -1.
 *
 * @tparam T An integral type (e.g., int, long, uint64_t).
 * @param value The integral value to examine.
 * @return int The index (0-based) of the highest set bit, or -1 if value is 0.
 *
 * @note A static assertion ensures that T is an integral type.
 */
template <class T>
inline int highestBit(T value)
{
    static_assert(std::is_integral<T>::value, "T must be an integral type");

    if (value == 0)
        return -1;

    int index = 0;
    while (value >>= 1)
        index++;

    return index;
}