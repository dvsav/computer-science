#pragma once

namespace cs
{
    /**
     * @brief Returns true if specified number @p num is prime.
     * @param num - integer number tested for primality.
     * @return true if specified number @p num is prime, false otherwise.
     */
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

    /**
     * @brief Returns a prime number greater than given @p x.
     * @param x
     * @return prime number greater than given @p x.
     */
    inline int NextPrime(int x)
    {
        while (true)
        {
            if (IsPrime(++x))
                return x;
        }
    }

    /**
     * @brief Returns a prime number smaller than given @p x.
     * @param x
     * @return prime number smaller than given @p x.
     */
    inline int PreviousPrime(int x)
    {
        while (x > 1)
        {
            if (IsPrime(--x))
                return x;
        }
        return 0;
    }
} // namespace cs
