#pragma once

namespace cs
{
    inline bool IsPrime(int num)
    {
        if (num <= 1)
            return false;

        for (int i = 2; i * i <= num; ++i)
        {
            if (num % i == 0)
                return false;
        }
        return true;
    }

    inline int NextPrime(int x)
    {
        while (true)
        {
            if (IsPrime(++x))
                return x;
        }
    }
} // namespace cs
