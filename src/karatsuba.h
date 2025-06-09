#pragma once

#include "requires.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

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
        // Array containing binary representation of the number.
        // E.g. value[0] represents the least significant 8 bits of the number (0...7),
        // value[1] represents the next 8 bits (8...15), etc.
        // The sign of the number is stored in its most significant bit.
        // If the number is negative 
        std::vector<uint8_t> value;

        /**
         * @brief Creates a VeryLongInteger and initializes it to 0.
         * @param size - the number of bytes in the binary representation of the number.
         */
        VeryLongInteger(size_t size) :
            value(/*count*/ size, /*val*/ 0)
        {}

        /**
         * @brief Removes redundant leading bytes from the value vector.
         *
         * This function trims the `value` vector by removing trailing bytes that are either 0x00 or 0xFF,
         * as long as there is more than one byte remaining. This is typically used to eliminate unnecessary
         * sign-extension bytes in a representation of a multi-byte integer.
         */
        void Prune()
        {
            while ((value.back() == 0x00 || value.back() == 0xFF) && value.size() > 1)
                value.pop_back();
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
            value(other.value)
        {}

        /**
         * @brief Move constructor
         */
        VeryLongInteger(VeryLongInteger&& other) noexcept :
            value(std::move(other.value))
        {}

        /**
         * @brief Copy assignment operator
         */
        VeryLongInteger& operator=(const VeryLongInteger& other)
        {
            value = other.value;
            return *this;
        }

        /**
         * @brief Move assignment operator
         */
        VeryLongInteger& operator=(VeryLongInteger&& other) noexcept
        {
            value = std::move(other.value);
            return *this;
        }

        /**
         * @brief Arithmetic shift left by a specified number @p N of bits.
         * @param N - the number of bits to shift to the left.
         * @return The shifted number.
         */
        VeryLongInteger operator<<(size_t N) const
        {
            const size_t newSize = (this->size() + N + 7) / 8;
            VeryLongInteger result(newSize);

            // Shift every byte of the number to the left by the specified number of bits N.
            // Most significant bits of an i-th byte get combined with (i+1)th byte.

            // result.value[0] = shiftLeft(this->value[0], N);
            // result.value[1] = shiftLeft(this->value[0], N - 8)  | shiftLeft(this->value[1], N);
            // result.value[2] = shiftLeft(this->value[0], N - 16) | shiftLeft(this->value[1], N - 8) | shiftLeft(this->value[2], N);
            // ...

            // Iterate over all bytes of the number
            for (size_t i = 0; i < result.size(); ++i)
            {
                // We consider only 2 bytes whose shifted bits can affect i-th byte of the resulting number:
                // The 1st byte is (i - N / 8)th and the 2nd one is (i - N / 8 - 1)th.
                const size_t j = N / 8;
                if (j <= i)
                {
                    result.value[i] |= shiftLeft(this->value[i - j], N - 8 * j);
                    if (N % 8 && j + 1 <= i)
                        result.value[i] |= shiftLeft(this->value[i - (j + 1)], N - 8 * (j + 1));
                }
            }
            result.Prune();

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
                size_t start = std::max(decIndex - 9, 0ull);
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
                size_t start = std::max(hexIndex - 2, 0ull);
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
            return value.size();
        }

        bool IsNonNegative() const
        {
            return !IsNegative();
        }

        bool IsNegative() const
        {
            // Check the most significant (sign) bit
            return (value.back() & 0x80) != 0;
        }

        bool IsZero() const
        {
            for (const auto byte : value)
            {
                if (byte)
                    return false;
            }
            return true;
        }

        bool IsPositive() const
        {
            return !IsNegative() && !IsZero();
        }

        VeryLongInteger Extended(size_t new_size) const
        {
            Requires::That(new_size >= size(), FUNCTION_INFO);

            if (new_size == size())
                return *this;

            VeryLongInteger result(new_size);
            std::copy(
                /*First*/ value.cbegin(),
                /*Last*/  value.cend(),
                /*Dest*/  result.value.begin());

            if (IsNegative())
            {
                for (size_t i = value.size(); i < new_size; ++i)
                    result.value[i] = uint8_t(0xFF);
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
            VeryLongInteger result(size());

            for (size_t i = 0; i < size(); ++i)
                result.value[i] = value[i] ^ 0b11111111;

            uint8_t carry = 1;
            for (size_t i = 0; i < size(); ++i)
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
            for (auto pbyte = value.rbegin(); pbyte != value.rend(); pbyte++)
                oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(*pbyte);
            return oss.str();
        }

        std::string ToBinary() const
        {
            std::ostringstream oss;
            for (auto pbyte = value.rbegin(); pbyte != value.rend(); pbyte++)
                oss << std::bitset<8>(*pbyte);
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

    /**
     * @brief Adds two VeryLongInteger numbers.
     * @param lhs - the first number.
     * @param rhs - the second number.
     * @return The sum of the two numbers.
     */
    inline VeryLongInteger operator+(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        size_t maxSize = std::max(lhs.size(), rhs.size());
        VeryLongInteger a = lhs.Extended(maxSize + 1);
        VeryLongInteger b = rhs.Extended(maxSize + 1);
        VeryLongInteger result(maxSize + 1);

        uint8_t carry = 0;
        for (size_t i = 0; i < result.size(); ++i)
        {
            uint16_t sum = a.value[i] + b.value[i] + carry;
            result.value[i] = static_cast<uint8_t>(sum & 0xFF);
            carry = sum >> 8;
        }
        result.Prune();

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
