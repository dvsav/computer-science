#include "requires.h"
#include "utility.h"

#include <algorithm> // for std::remove
#include <fstream>   // for std::ifstream
#include <iostream>  // for std::cerr
#include <string>    // for std::string

bool files_textually_equal(
    const std::string& filePathA,
    const std::string& filePathB)
{
    std::ifstream f1(filePathA);
    std::ifstream f2(filePathB);

    if (f1.fail())
    {
        std::cerr << __FUNCTION__ << ": couldn't open file " << filePathA << std::endl;
        return false; // file problem
    }

    if (f2.fail())
    {
        std::cerr << __FUNCTION__ << ": couldn't open file " << filePathB << std::endl;
        return false; // file problem
    }

    std::string s1;
    std::string s2;
    while (true)
    {
        std::getline(f1, s1);
        std::getline(f2, s2);

        if ((bool)f1 != (bool)f2)
            return false;
        else if (!f1)
            return true;

        s1.erase(std::remove(s1.begin(), s1.end(), '\r'), s1.end());
        s2.erase(std::remove(s2.begin(), s2.end(), '\r'), s2.end());
        if (s1 != s2)
        {
            return false;
        }
    }
}

bool skip_comment(
    std::istream& is,
    const std::string& comment_begins_with)
{
    Requires::That(comment_begins_with.size() > 0, FUNCTION_INFO);

    skip_whitespace(is);

    // read the 1st character of comment_begins_with
    int ch = is.get();
    if (ch != comment_begins_with[0])
    {
        is.unget();
        return false;
    }

    // read the rest of comment_begins_with
    for (size_t i = 1; i < comment_begins_with.size(); i++)
    {
        ch = is.get();
        if (ch != comment_begins_with[i])
        {
            is.unget();
            is.clear(std::ios_base::failbit);
            return false;
        }
    }

    // read and doscard a single line
    std::string line;
    std::getline(is, line);

    return true;
}