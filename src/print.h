#include <iostream>

namespace sc
{
    template <typename TIterator>
    void print(TIterator begin, TIterator end, const std::string& delimeter)
    {
        for (auto i = begin; i != end; i++)
            std::cout << *i << delimeter;
    }
}
