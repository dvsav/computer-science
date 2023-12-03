#pragma once

#include "requires.h"

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
std::istream& operator>>(
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

template <typename TIterator>
void print(
    std::ostream& os,
    TIterator begin,
    TIterator end,
    const std::string& delimeter)
{
    for (auto i = begin; i != end; i++)
        os << *i << delimeter;
}

template <typename TIterator>
void print(
    TIterator begin,
    TIterator end,
    const std::string& delimeter)
{
    print(std::cout, begin, end, delimeter);
}