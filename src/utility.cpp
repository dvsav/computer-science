#include "utility.h"

#include <algorithm> // for std::remove
#include <fstream>   // for std::ifstream
#include <iostream>  // for std::cerr
#include <string>    // for std::string

namespace cs
{
    bool files_textually_equal(
        const std::string& filePathA,
        const std::string& filePathB)
    {
        std::ifstream f1(filePathA);
        std::ifstream f2(filePathB);

        if (f1.fail() || f2.fail())
        {
            std::cerr << __FUNCTION__ << ": file problem" << std::endl;
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
}
