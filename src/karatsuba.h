#pragma once

#include "requires.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

/*
TODO
1. Create templated constructor to convert from any integral type to VeryLongInteger.
2. Implement bitwise operators (&, |).
3. Add doxygen comments to all methods.
4. Check where const and move can be applied.
*/

namespace cs
{
    class VeryLongInteger;

    VeryLongInteger Power(
        const VeryLongInteger& val,
        size_t power);

    /**
     * @class VeryLongInteger
     * @brief Class representing an arbitrarily long signed integer number.
     */
    class VeryLongInteger
    {
    private:
        // The number of bytes in the binary representation of the number.
        size_t nBytes;

        // Array containing binary representation of the number.
        // E.g. value[0] represents the least significant 8 bits of the number (0...7),
        // value[1] represents the next 8 bits (8...15), etc.
        // The sign of the number is stored in its most significant bit.
        // If the number is negative 
        uint8_t* value;

        /**
         * @brief Creates a VeryLongInteger and initializes it to 0.
         * @param size - the number of bytes in the binary representation of the number.
         */
        VeryLongInteger(size_t size) :
            nBytes(size),
            value(new uint8_t[nBytes])
        {
            Requires::ArgumentPositive(size, NAMEOF(size), FUNCTION_INFO);
            for (size_t i = 0; i < nBytes; i++)
                value[i] = 0;
        }

        static uint8_t shiftLeft(uint8_t val, int offset)
        {
            if (offset > 0)
                return static_cast<uint8_t>(val << offset);
            else if (offset < 0)
                return static_cast<uint8_t>(val >> offset);
            else
                return val;
        }

        static int highestBit(uint8_t value)
        {
            if (value == 0)
                return -1;

            int index = 0;
            while (value >>= 1)
                ++index;

            return index;
        }

    public:
        /**
         * @brief Copy constructor
         */
        VeryLongInteger(const VeryLongInteger& other) :
            nBytes(other.nBytes),
            value(new uint8_t[other.nBytes])
        {
            std::copy(other.value, other.value + nBytes, value);
        }

        /**
         * @brief Move constructor
         */
        VeryLongInteger(VeryLongInteger&& other) noexcept :
            nBytes(other.nBytes),
            value(other.value)
        {
            other.value = nullptr;
            other.nBytes = 0;
        }

        /**
         * @brief Destructor
         */
        ~VeryLongInteger()
        {
            if (value)
            {
                delete[] value;
                value = nullptr;
                nBytes = 0;
            }
        }

        /**
         * @brief Copy assignment operator
         */
        VeryLongInteger& operator=(const VeryLongInteger& other)
        {
            if (this != &other)
            {
                if (nBytes != other.size())
                {
                    delete[] value;
                    nBytes = other.nBytes;
                    value = new uint8_t[nBytes];
                }
                std::copy(other.value, other.value + nBytes, value);
            }
            return *this;
        }

        /**
         * @brief Move assignment operator
         */
        VeryLongInteger& operator=(VeryLongInteger&& other) noexcept
        {
            if (this != &other)
            {
                delete[] value;
                value = other.value;
                nBytes = other.nBytes;
                other.value = nullptr;
                other.nBytes = 0;
            }
            return *this;
        }

        VeryLongInteger operator<<(size_t N) const
        {
            const size_t newSize = (this->size() + 7) / 8;
            VeryLongInteger result(newSize);

            // result.value[0] = shiftLeft(this->value[0], N);
            // result.value[1] = shiftLeft(this->value[0], N - 8)  | shiftLeft(this->value[1], N);
            // result.value[2] = shiftLeft(this->value[0], N - 16) | shiftLeft(this->value[1], N - 8) | shiftLeft(this->value[2], N);
            // ...

            for (size_t i = 0; i < result.size(); ++i)
            {
                for (size_t j = 0; j <= i; ++j)
                {
                    result.value[i] |= shiftLeft(this->value[i - j], N - 8 * j);
                }
            }

            return result;
        }

    public:
        template <typename T>
        static VeryLongInteger FromInteger(T value)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            size_t nBytes = sizeof(T);
            VeryLongInteger result(nBytes);
            for (size_t i = 0; i < nBytes; ++i)
                result.value[i] = static_cast<uint8_t>(value >> (8 * i));
            return result;
        }

        static VeryLongInteger FromDecimal(const std::string& decimal)
        {
            std::string cleaned = decimal;
            bool isPositive = true;
            if (decimal.rfind("-", 0) == 0)
            {
                cleaned = decimal.substr(1);
                isPositive = false;
            }
            else if (decimal.rfind("+", 0) == 0)
            {
                cleaned = decimal.substr(1);
            }
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            VeryLongInteger result(1);
            size_t decIndex = cleaned.length();
            while (true)
            {
                size_t start = std::max(decIndex - 9, 0ul);
                std::string str = cleaned.substr(start, decIndex - start);
                unsigned long val = std::stoul(/*str*/ str, /*pos*/ nullptr, /*base*/ 10);
                result = result +
                    VeryLongInteger::FromInteger(val) *
                        Power(/*val*/ VeryLongInteger::FromInteger(10ul), /*power*/ cleaned.length() - decIndex);
                decIndex = start;
                if (decIndex <= 0)
                    break;
            }

            return isPositive ? result : -result;
        }

        static VeryLongInteger FromHexadecimal(const std::string& hex)
        {
            // number with removed prefix "0x" or "OX"
            std::string cleaned = hex;
            if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0)
                cleaned = hex.substr(2);
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            // total number of bytes required to store the number
            size_t lenBytes = (cleaned.length() + 1) / 2;
            VeryLongInteger result(lenBytes);

            // iterate over the 2-hex digits (1 byte) groups of symbols in the string
            // moving from the end towards the beginning of the string
            size_t hexIndex = cleaned.length();
            for (size_t i = 0; i < lenBytes; ++i)
            {
                size_t start = std::max(hexIndex - 2, 0ul);
                std::string byteStr = cleaned.substr(start, hexIndex - start);
                // convert hex string to unsigned integer
                result.value[i] = static_cast<uint8_t>(
                    std::stoul(/*str*/ byteStr, /*pos*/ nullptr, /*base*/ 16)
                );
                hexIndex = start;
            }
            return result;
        }

        static VeryLongInteger FromBinary(const std::string& bin)
        {
            // number with removed prefix "0b" or "0B"
            std::string cleaned = bin;
            if (bin.rfind("0b", 0) == 0 || bin.rfind("0B", 0) == 0)
                cleaned = bin.substr(2);
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            // total number of bytes required to store the number
            size_t lenBytes = (cleaned.length() + 7) / 8;
            VeryLongInteger result(lenBytes);

            // iterate over the 8-bit (1 byte) groups of symbols in the string
            // moving from the end towards the beginning of the string
            size_t bitIndex = cleaned.length();
            for (size_t i = 0; i < lenBytes; ++i)
            {
                size_t start = std::max(static_cast<int>(bitIndex) - 8, 0);
                std::string byteStr = cleaned.substr(start, bitIndex - start);
                // convert binary string to unsigned integer
                result.value[i] = static_cast<uint8_t>(
                    std::stoul(/*str*/ byteStr, /*pos*/ nullptr, /*base*/ 2)
                );
                bitIndex = start;
            }
            return result;
        }

    public:
        /**
         * @brief Returns the number of bytes in the binary representation of the number.
         */
        size_t size() const
        {
            return nBytes;
        }

        bool IsNonNegative() const
        {
            return !IsNegative();
        }

        bool IsNegative() const
        {
            return (value[nBytes - 1] & 0x80) != 0;
        }

        bool IsPositive() const
        {
            if (IsNegative())
                return false;

            for (size_t i = 0; i < nBytes; ++i)
            {
                if (value[i])
                    return true;
            }

            return false;
        }

        bool IsZero() const
        {
            for (size_t i = 0; i < nBytes; ++i)
            {
                if (value[i])
                    return false;
            }
            return true;
        }

        VeryLongInteger Extend(size_t new_size) const
        {
            Requires::That(new_size >= nBytes, FUNCTION_INFO);

            if (new_size == nBytes)
                return *this;

            VeryLongInteger result(new_size);
            std::copy(result.value, result.value + nBytes, value);

            if (value[nBytes - 1] & 0b10000000)
            {
                // negative number
                for (size_t i = nBytes; i < new_size; ++i)
                    result.value[i] = 0b11111111;
            }
            else
            {
                // positive number
                for (size_t i = nBytes; i < new_size; ++i)
                    result.value[i] = 0b00000000;
            }

            return result;
        }

        VeryLongInteger Abs() const
        {
            if (this->IsNonNegative())
                return *this;
            else
                return -(*this);
        }

        VeryLongInteger operator-() const
        {
            VeryLongInteger result(nBytes);

            for (size_t i = 0; i < nBytes; ++i)
                result.value[i] ^= 0b11111111;

            uint8_t carry = 1;
            for (size_t i = 0; i < nBytes; ++i)
            {
                uint16_t sum = result.value[i] + carry;
                result.value[i] = static_cast<uint8_t>(sum & 0xFF);
                carry = sum >> 8;
            }

            return result;
        }

        std::string ToDecimal() const
        {
            VeryLongInteger copy = *this;
            const VeryLongInteger zero = FromInteger(0);
            std::string result = "";

            while (copy.IsPositive())
            {
                VeryLongInteger quotient = copy / FromInteger(10);
                VeryLongInteger remainder = copy - quotient * FromInteger(10);
                result.insert(/*pos*/ 0, /*str*/ std::to_string(value[0]));
                copy = quotient;
            }

            return result;
        }

        std::string ToHexadecimal() const
        {
            std::ostringstream oss;
            for (size_t i = 0; i < nBytes; ++i)
                oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)value[nBytes - 1 - i];
            return oss.str();
        }

        std::string ToBinary() const
        {
            std::ostringstream oss;
            for (size_t i = 0; i < nBytes; ++i)
                oss << std::bitset<8>(value[nBytes - 1 - i]);
            return oss.str();
        }

        void SetBit(size_t bit)
        {
            const size_t byteNum = bit / 8;
            const size_t bitNum = bit % 8;
            this->value[byteNum] |= (1 << bitNum);
        }

        void ClearBit(size_t bit)
        {
            const size_t byteNum = bit / 8;
            const size_t bitNum = bit % 8;
            this->value[byteNum] &= ~(1 << bitNum);
        }

    public:
        friend VeryLongInteger operator+(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator-(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator*(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator/(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);
    };

    inline VeryLongInteger operator+(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        size_t maxSize = std::max(lhs.size(), rhs.size());
        VeryLongInteger a = lhs.Extend(maxSize);
        VeryLongInteger b = rhs.Extend(maxSize);
        VeryLongInteger result(maxSize + 1);

        uint8_t carry = 0;
        for (size_t i = 0; i < maxSize; ++i)
        {
            uint16_t sum = a.value[i] + b.value[i] + carry;
            result.value[i] = static_cast<uint8_t>(sum & 0xFF);
            carry = sum >> 8;
        }

        return result;
    }

    inline VeryLongInteger operator-(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return lhs + (-rhs);
    }

    inline VeryLongInteger operator*(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        VeryLongInteger smaller = lhs.size() < rhs.size() ? lhs : rhs;
        VeryLongInteger larger = lhs.size() >= rhs.size() ? lhs : rhs;
        VeryLongInteger result(smaller.size() + larger.size());
        for (size_t i = 0; i < smaller.size(); ++i)
        {
            for (size_t bitNum = 0; bitNum < 8; ++bitNum)
            {
                uint8_t smallerBit = (smaller.value[i] >> bitNum) & 0x01;
                if (smallerBit)
                    result = result + (result << (bitNum + 8 * i));
            }
        }
        return result;
    }

    inline VeryLongInteger operator/(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        int lhsHighestBit = VeryLongInteger::highestBit(lhs.value[lhs.size() - 1]);
        int rhsHighestBit = VeryLongInteger::highestBit(rhs.value[lhs.size() - 1]);
        if (lhsHighestBit < rhsHighestBit)
            return VeryLongInteger::FromInteger(0);

        VeryLongInteger result((lhsHighestBit - rhsHighestBit + 7) / 8);
        VeryLongInteger copy = lhs;
        for (int i = lhsHighestBit - rhsHighestBit; i >= 0; --i)
        {
            copy = copy - (rhs << i);
            if (copy.IsPositive())
                result.SetBit(i);
        }
        return result;
    }

    bool operator>(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsPositive();
    }

    bool operator<(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (rhs - lhs).IsPositive();
    }

    bool operator>=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsNonNegative();
    }

    bool operator<=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (rhs - lhs).IsNonNegative();
    }

    bool operator==(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsZero();
    }

    bool operator!=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return !(lhs == rhs);
    }

    inline VeryLongInteger Power(
        const VeryLongInteger& val,
        size_t power)
    {
        Requires::ArgumentNotNegative(power, NAMEOF(power), FUNCTION_INFO);
        VeryLongInteger result = VeryLongInteger::FromInteger(1);
        for (size_t i = 0; i < power; ++i)
            result = result * val;
        return result;
    }
}
