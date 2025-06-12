#pragma once

#include "requires.h"
#include "utility.h"

#include <algorithm>
#include <bitset>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

/*
TODO:
Cleanup and optimize unit tests.
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
        // If the number is negative it's stored in two's complement format.
        std::vector<uint8_t> value;

        /**
         * @brief Creates a VeryLongInteger with @p size bytes and initializes all of its bytes to a specified value @p val.
         * @param size The number of bytes in the binary representation of the number.
         * @param val The value used to intialize each byte in the binary representation of the number.
         */
        VeryLongInteger(size_t size, uint8_t val) :
            value(/*count*/ size, /*val*/ val)
        {
            Requires::ArgumentPositive(size, NAMEOF(size), FUNCTION_INFO);
        }

        /**
         * @brief Removes redundant leading bytes from the value vector.
         *
         * This function trims the `value` vector by removing trailing bytes that are either 0x00 or 0xFF,
         * as long as there is more than one byte remaining. This is typically used to eliminate unnecessary
         * sign-extension bytes in a representation of a multi-byte integer.
         */
        void Prune()
        {
            while (value.size() > 1)
            {
                if ( value[size() - 1] == 0x00 && ( (value[size() - 2] & 0b10000000) == 0 ) )
                    value.pop_back();
                else if ( value[size() - 1] == 0xFF && ( (value[size() - 2] & 0b10000000) != 0 ) )
                    value.pop_back();
                else
                    break;
            }
        }

    public:
        /**
         * @brief Constructs a VeryLongInteger from an integral value @p val.
         * @tparam T An integral type (e.g., int, long, uint64_t).
         * @param val The integral value from which a VeryLongInteger is constructed.
         * @note A static assertion ensures that T is an integral type.
         */
        template <typename T>
        VeryLongInteger(T val) :
            value(/*count*/ sizeof(T), /*val*/ 0)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            for (size_t i = 0; i < value.size(); i++)
                value[i] = static_cast<uint8_t>(val >> (8 * i));
        }

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
         * Preserves the sign of the number.
         * @param N the number of bits to shift to the left.
         * @return The shifted number.
         */
        VeryLongInteger operator<<(size_t N) const
        {
            Requires::ArgumentNotNegative(N, NAMEOF(N), FUNCTION_INFO);
            if (N == 0)
                return *this;

            const size_t newSize = (this->size() * 8 + N + 7) / 8;
            VeryLongInteger result(/*size*/ newSize, /*val*/ 0);

            // Shift every byte of the number to the left by the specified number of bits N.

            // Iterate over all bytes of the number
            for (size_t i = 0; i < result.size(); i++)
            {
                // We consider only 2 bytes whose shifted bits can affect i-th byte of the resulting number:

                // The 1st byte is (i - N/8)th ...
                int j = i - N / 8;
                if (j >= 0 && j < (int)this->size())
                    result.value[i] |= shiftLeft(this->value[j], N % 8);

                // The 2nd byte is (i - N/8 - 1)th.
                j = j - 1;
                if (N % 8 && j >= 0 && j < (int)this->size())
                    result.value[i] |= shiftLeft(this->value[j], N % 8 - 8);
            }
            
            if (this->IsNegative())
            {
                // Fill the most significant bits of the result with 1's
                result.value.back() |= (0xFF << (N % 8));
            }
            result.Prune();

            return result;
        }

        /**
         * @brief Arithmetic shift right by a specified number @p N of bits.
         * Preserves the sign of the number.
         * @param N the number of bits to shift to the right.
         * @return The shifted number.
         */
        VeryLongInteger operator>>(size_t N) const
        {
            Requires::ArgumentNotNegative(N, NAMEOF(N), FUNCTION_INFO);
            if (N == 0)
                return *this;

            const size_t newSize = (this->size() * 8 - N + 7) / 8;
            VeryLongInteger result(/*size*/ newSize, /*val*/ 0);

            // Shift every byte of the number to the right by the specified number of bits N.

            // Iterate over all bytes of the number
            for (size_t i = 0; i < result.size(); i++)
            {
                // We consider only 2 bytes whose shifted bits can affect i-th byte of the resulting number:

                // The 1st byte is (i + N/8)th ...
                int j = i + N / 8;
                if (j >= 0 && j < (int)this->size())
                    result.value[i] |= shiftRight(this->value[j], N % 8);

                // The 2nd byte is (i + N/8 + 1)th.
                j = j + 1;
                if (N % 8 && j >= 0 && j < (int)this->size())
                    result.value[i] |= shiftRight(this->value[j], N % 8 - 8);
            }
            
            if (this->IsNegative())
            {
                // Fill the most significant bits of the result with 1's
                result.value.back() |= (0xFF << (8 - N % 8));
            }
            result.Prune();

            return result;
        }

        /**
         * @brief Bitwise NOT operator for VeryLongInteger.
         *
         * Creates a copy of the current VeryLongInteger and applies the bitwise NOT operation
         * to each byte in its internal value representation. Returns the resulting VeryLongInteger.
         *
         * @return A new VeryLongInteger with each byte inverted.
         */
        VeryLongInteger operator~() const
        {
            VeryLongInteger result(*this);
            for (auto& byte : result.value)
                byte = ~byte;
            return result;
        }

    public:
        /**
         * @brief Constructs a VeryLongInteger from a string
         * representing the number in decimal format.
         * The string may optionally contain a sign "+" or "-".
         * @param decimal the string representing the number in decimal format.
         * @return The resulting VeryLongInteger object.
         */
        static VeryLongInteger FromDecimal(const std::string& decimal)
        {
            size_t startPosition = 0;

            // exclude leading spaces
            while (std::isspace(decimal[startPosition]))
                startPosition++;

            // search for the sign
            bool isNegative = false;
            if (decimal.rfind('-', startPosition) == startPosition)
            {
                isNegative = true;
                startPosition++;
            }
            else if (decimal.rfind('+', startPosition) == startPosition)
                startPosition++;

            // string containing only decimal digits
            const std::string cleaned = decimal.substr(startPosition);
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            VeryLongInteger result{uint8_t(0)};
            size_t decIndex = cleaned.length();
            while (true)
            {
                // Convert groups of 9 decimal digits (less than 1 billion which fits to a 32 bit integer) to a number and add to the result.
                int start = std::max<int>(decIndex - 9, 0);
                std::string str = cleaned.substr(start, decIndex - start);
                unsigned long val = std::stoul(/*str*/ str, /*pos*/ nullptr, /*base*/ 10);
                result = result +
                    VeryLongInteger{val} *
                        Power(/*val*/ VeryLongInteger{uint8_t(10)}, /*power*/ cleaned.length() - decIndex);
                decIndex = start;
                if (decIndex <= 0)
                    break;
            }

            return isNegative ? -result : result;
        }

        /**
         * @brief Constructs a VeryLongInteger from a string
         * representing the number in hexadecimal format.
         * The string may optionally have a prefix "0x" or "0X".
         * The string is always interpreted as an unsigned integer.
         * @param hex the string representing the number in hexadecimal format.
         * @return The resulting VeryLongInteger object.
         */
        static VeryLongInteger FromHexadecimal(const std::string& hex)
        {
            size_t startPosition = 0;

            // exclude leading spaces
            while (std::isspace(hex[startPosition]))
                startPosition++;

            // search for prefix "0x" or "OX"
            if (hex.rfind("0x", startPosition) == startPosition || hex.rfind("0X", startPosition) == startPosition)
                startPosition += 2;

            // string containing only hex digits
            const std::string cleaned = hex.substr(startPosition);
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            // total number of bytes required to store the number
            const size_t lenBytes = (cleaned.length() + 1) / 2;
            VeryLongInteger result(/*size*/ lenBytes, /*val*/ 0);

            // iterate over the 2-hex digits (1 byte) groups of symbols in the string
            // moving from the end towards the beginning of the string
            size_t hexIndex = cleaned.length();
            for (size_t i = 0; i < lenBytes; i++)
            {
                const size_t start = std::max<size_t>(hexIndex - 2, 0);
                std::string byteStr = cleaned.substr(start, hexIndex - start);
                // convert hex string to unsigned integer
                result.value[i] = static_cast<uint8_t>(
                    std::stoul(/*str*/ byteStr, /*pos*/ nullptr, /*base*/ 16)
                );
                hexIndex = start;
            }
            return result;
        }

        /**
         * @brief Constructs a VeryLongInteger from a string
         * representing the number in binary format.
         * The string may optionally have a prefix "0b" or "0B".
         * The string is always interpreted as an unsigned integer.
         * @param bin the string representing the number in binary format.
         * @return The resulting VeryLongInteger object.
         */
        static VeryLongInteger FromBinary(const std::string& bin)
        {
            size_t startPosition = 0;

            // exclude leading spaces
            while (std::isspace(bin[startPosition]))
                startPosition++;

            // search for prefix "0b" or "0B"
            if (bin.rfind("0b", startPosition) == startPosition || bin.rfind("0B", startPosition) == startPosition)
                startPosition += 2;

            // string containing only binary digits
            const std::string cleaned = bin.substr(startPosition);
            Requires::That(cleaned.length() > 0, FUNCTION_INFO);

            // total number of bytes required to store the number
            const size_t lenBytes = (cleaned.length() + 7) / 8;
            VeryLongInteger result(/*size*/ lenBytes, /*val*/ 0);

            // iterate over the 8-bit (1 byte) groups of symbols in the string
            // moving from the end towards the beginning of the string
            size_t bitIndex = cleaned.length();
            for (size_t i = 0; i < lenBytes; i++)
            {
                const size_t start = std::max(static_cast<int>(bitIndex) - 8, 0);
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

        /**
         * @brief Returns the index of the highest nonzero bit of the number.
         * 
         * This function computes the position of the most significant bit set to 1 in the number
         * If the number equals 0, the function returns -1.
         * 
         * @return int The index (0-based) of the highest set bit, or -1 if the number is 0.
         */
        int HighestBit() const
        {
            for (int i = size() - 1; i >= 0; i--)
            {
                const int bit = highestBit(value[i]);
                if (bit >= 0)
                    return bit + i * 8;
            }
            return -1;
        }

        /**
         * @brief Returns true if the number is non-negative.
         */
        bool IsNonNegative() const
        {
            return !IsNegative();
        }

        /**
         * @brief Returns true if the number is negative.
         */
        bool IsNegative() const
        {
            // Check the most significant (sign) bit
            return (value.back() & 0b10000000) != 0;
        }

        /**
         * @brief Returns true if the number is zero.
         */
        bool IsZero() const
        {
            for (const auto byte : value)
            {
                if (byte)
                    return false;
            }
            return true;
        }

        /**
         * @brief Returns true if the number is positive.
         */
        bool IsPositive() const
        {
            return !IsNegative() && !IsZero();
        }

        /**
         * @brief Returns a sign-extended copy of the number.
         * @param new_size The size of extended number
         * (must be greater or equal to the size of the original number).
         * @return The extended number.
         */
        VeryLongInteger Extended(size_t new_size) const
        {
            Requires::That(new_size >= size(), FUNCTION_INFO);

            if (new_size == this->size())
                return *this;

            VeryLongInteger result(/*size*/ new_size, /*val*/ 0);
            std::copy(
                /*First*/ this->value.cbegin(),
                /*Last*/  this->value.cend(),
                /*Dest*/  result.value.begin());

            if (this->IsNegative())
            {
                // Fill the most significant bytes of the result with 1's
                for (size_t i = value.size(); i < new_size; i++)
                    result.value[i] = uint8_t(0xFF);
            }

            return result;
        }

        /**
         * @brief Returns the absoulute value of the number.
         */
        VeryLongInteger Abs() const
        {
            if (this->IsNonNegative())
                return *this;
            else
                return -(*this);
        }

        /**
         * @brief Returns the negated copy of the number.
         */
        VeryLongInteger operator-() const
        {
            VeryLongInteger result(/*size*/ size(), /*val*/ 0);

            for (size_t i = 0; i < size(); i++)
                result.value[i] = value[i] ^ 0b11111111;

            uint8_t carry = 1;
            for (size_t i = 0; i < size(); i++)
            {
                const uint16_t sum = result.value[i] + carry;
                result.value[i] = static_cast<uint8_t>(sum & 0xFF);
                carry = sum >> 8;
            }

            return result;
        }

        /**
         * @brief Returns a string representing the number in decimal format.
         * The number is always treated as signed.
         * "-" is added at the front if the number is negative.
         */
        std::string ToDecimal() const
        {
            if (this->IsZero())
                return "0";

            VeryLongInteger copy = this->Abs();
            const VeryLongInteger ten{uint8_t(10)};
            std::string result = "";

            while (copy.IsPositive())
            {
                VeryLongInteger quotient = copy / ten;
                const VeryLongInteger remainder = copy - quotient * ten;
                result.insert(/*pos*/ 0, /*str*/ std::to_string(remainder.value[0]));
                copy = std::move(quotient);
            }

            if (this->IsNegative())
                result.insert(/*pos*/ result.begin(), /*char*/ '-');

            return result;
        }

        /**
         * @brief Returns a string representing the number in hex format.
         * The number is always treated as unsigned
         * (no "-" sign is added even if the unmber is negative).
         */
        std::string ToHexadecimal() const
        {
            std::ostringstream oss;
            for (auto pbyte = value.rbegin(); pbyte != value.rend(); ++pbyte)
                oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(*pbyte);
            return oss.str();
        }

        /**
         * @brief Returns a string representing the number in binary format.
         * The number is always treated as unsigned.
         * (no "-" sign is added even if the unmber is negative).
         */
        std::string ToBinary() const
        {
            std::ostringstream oss;
            for (auto pbyte = value.rbegin(); pbyte != value.rend(); ++pbyte)
                oss << std::bitset<8>(*pbyte);
            return oss.str();
        }

        /**
         * @brief Sets a specified bit of the number to 1.
         * @param bit The position of the bit to be set to 1.
         */
        void SetBit(size_t bit)
        {
            const size_t byteNum = bit / 8;
            Requires::That(byteNum < size(), FUNCTION_INFO);
            const size_t bitNum = bit % 8;
            this->value[byteNum] |= (1 << bitNum);
        }

        /**
         * @brief Sets a specified bit of the number to 0.
         * @param bit The position of the bit to be set to 0.
         */
        void ClearBit(size_t bit)
        {
            const size_t byteNum = bit / 8;
            Requires::That(byteNum < size(), FUNCTION_INFO);
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

        friend VeryLongInteger Karatsuba(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator/(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator|(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator&(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);

        friend VeryLongInteger operator^(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);
    };

    /**
     * @brief Adds two VeryLongInteger numbers.
     * @param lhs the first number.
     * @param rhs the second number.
     * @return The sum of the two numbers.
     */
    inline VeryLongInteger operator+(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const size_t maxSize = std::max(lhs.size(), rhs.size());
        const VeryLongInteger a = lhs.Extended(maxSize + 1);
        const VeryLongInteger b = rhs.Extended(maxSize + 1);
        VeryLongInteger result(/*size*/ maxSize + 1, /*val*/ 0);

        uint8_t carry = 0;
        for (size_t i = 0; i < result.size(); i++)
        {
            const uint16_t sum = a.value[i] + b.value[i] + carry;
            result.value[i] = static_cast<uint8_t>(sum & 0xFF);
            carry = sum >> 8;
        }
        result.Prune();

        return result;
    }

    /**
     * @brief Subtracts @p rhs from @p lhs.
     * @param lhs The minuend.
     * @param rhs The subtrahend.
     * @return The difference of the minuend and the subtrahend.
     */
    inline VeryLongInteger operator-(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return lhs + (-rhs);
    }

    /**
     * @brief Multiplies @p lhs by @p rhs.
     * Time complexity: O(N*M) where N, M are the count of
     * binary digits in the numbers being mupltiplied.
     * @param lhs The mutiplicand.
     * @param rhs The multiplier.
     * @return The product of the mutiplicand and the multiplier.
     */
    inline VeryLongInteger operator*(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const VeryLongInteger& smaller = lhs.size() < rhs.size() ? lhs : rhs;
        const VeryLongInteger& larger = lhs.size() >= rhs.size() ? lhs : rhs;
        VeryLongInteger result(/*size*/ smaller.size() + larger.size(), /*val*/ 0);
        // Iterate over the bytes of the smaller number
        for (size_t i = 0; i < smaller.size(); i++)
        {
            for (size_t bitNum = 0; bitNum < 8; bitNum++)
            {
                const uint8_t smallerBit = (smaller.value[i] >> bitNum) & 0x01;
                if (smallerBit)
                    result = result + (larger << (8 * i + bitNum));
            }
        }
        return result;
    }

    /**
     * @brief Multiplies @p lhs by @p rhs using Karatsuba algorithm.
     * Time complexity: O(N^log2(3)) ~ O(N^1.585) where N is the number of
     * binary digits in the largest of the numbers being mupltiplied.
     * @param lhs The mutiplicand.
     * @param rhs The multiplier.
     * @return The product of the mutiplicand and the multiplier.
     */
    inline VeryLongInteger Karatsuba(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        if (lhs.size() == 1 && rhs.size() == 1)
            return lhs * rhs;

        size_t maxSize = std::max(lhs.size(), rhs.size());
        if (maxSize % 2) maxSize++; // maxSize should be even
        const size_t N = maxSize * 8;
        const VeryLongInteger x = lhs.Extended(maxSize);
        const VeryLongInteger y = rhs.Extended(maxSize);

        // x = a * 2^(N/2) + b
        // y = c * 2^(N/2) + d
        // x * y = ac * 2^N + (ad + bc) * 2^(N/2) + bd
        // 1. Compute: ac
        // 2. Compute: bd
        // 3. Compute: (ad + bc) = (a + b)(c + d) - ac - bd

        VeryLongInteger a = x >> (N / 2);
        VeryLongInteger b = y & VeryLongInteger(/*size*/ maxSize / 2, /*val*/ 0xFF);
        b.Prune();

        VeryLongInteger c = y >> (N / 2);
        VeryLongInteger d = y & VeryLongInteger(/*size*/ maxSize / 2, /*val*/ 0xFF);
        d.Prune();

        VeryLongInteger ac = Karatsuba(a, c);
        VeryLongInteger bd = Karatsuba(b, d);
        VeryLongInteger ad_bc = Karatsuba(a + b, c + d) - ac - bd;

        return (ac << N) + (ad_bc << (N/2)) + bd;
    }

    /**
     * @brief Divides @p lhs by @p rhs (integral division).
     * The quatient is always truncated towards zero,
     * e.g. 10 / 3 = 3, -10 / 3 = -3, 10 / -3 = -3.
     * @param lhs The dividend.
     * @param rhs The divisor.
     * @return The quatient of the dividend and the divisor.
     */
    inline VeryLongInteger operator/(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const bool resultIsNegative = lhs.IsNegative() ^ rhs.IsNegative();

        VeryLongInteger lhs_copy = lhs.Abs();
        VeryLongInteger rhs_copy = rhs.Abs();

        const int lhsHighestBit = lhs_copy.HighestBit();
        const int rhsHighestBit = rhs_copy.HighestBit();

        if (lhsHighestBit < rhsHighestBit)
            return VeryLongInteger{uint8_t(0)};

        VeryLongInteger result(
            /*size*/ std::max((lhsHighestBit - rhsHighestBit + 7) / 8, 1),
            /*val*/ 0);

        for (int i = lhsHighestBit - rhsHighestBit; i >= 0; --i)
        {
            VeryLongInteger remainder = lhs_copy - (rhs_copy << i);
            if (remainder.IsNonNegative())
            {
                result.SetBit(i);
                lhs_copy = std::move(remainder);
            }
        }
        return resultIsNegative ? -result : result;
    }

    /**
     * @brief Bitwise OR operator for VeryLongInteger objects.
     *
     * Performs a bitwise OR operation between two VeryLongInteger instances.
     * The operands are first extended to the same size (the maximum of their sizes plus one)
     * to ensure correct bitwise operation across all digits. The result is a new VeryLongInteger
     * containing the bitwise OR of the corresponding digits.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return VeryLongInteger The result of the bitwise OR operation.
     */
    inline VeryLongInteger operator|(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const size_t maxSize = std::max(lhs.size(), rhs.size());
        const VeryLongInteger a = lhs.Extended(maxSize + 1);
        const VeryLongInteger b = rhs.Extended(maxSize + 1);
        VeryLongInteger result(/*size*/ maxSize + 1, /*val*/ 0);

        for (size_t i = 0; i < result.size(); i++)
        {
            result.value[i] = a.value[i] | b.value[i];
        }
        return result;
    }

    /**
     * @brief Bitwise AND operator for VeryLongInteger objects.
     *
     * Performs a bitwise AND operation between two VeryLongInteger instances.
     * The operands are first extended to the same size (the maximum of their sizes plus one)
     * to ensure correct bitwise operation across all digits. The result is a new VeryLongInteger
     * containing the bitwise AND of the corresponding digits.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return VeryLongInteger The result of the bitwise AND operation.
     */
    inline VeryLongInteger operator&(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const size_t maxSize = std::max(lhs.size(), rhs.size());
        const VeryLongInteger a = lhs.Extended(maxSize + 1);
        const VeryLongInteger b = rhs.Extended(maxSize + 1);
        VeryLongInteger result(/*size*/ maxSize + 1, /*val*/ 0);

        for (size_t i = 0; i < result.size(); i++)
        {
            result.value[i] = a.value[i] & b.value[i];
        }
        return result;
    }

    /**
     * @brief Bitwise XOR operator for VeryLongInteger objects.
     *
     * Performs a bitwise XOR operation between two VeryLongInteger instances.
     * The operands are first extended to the same size (the maximum of their sizes plus one)
     * to ensure correct bitwise operation across all digits. The result is a new VeryLongInteger
     * containing the bitwise XOR of the corresponding digits.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return VeryLongInteger The result of the bitwise XOR operation.
     */
    inline VeryLongInteger operator^(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        const size_t maxSize = std::max(lhs.size(), rhs.size());
        const VeryLongInteger a = lhs.Extended(maxSize + 1);
        const VeryLongInteger b = rhs.Extended(maxSize + 1);
        VeryLongInteger result(/*size*/ maxSize + 1, /*val*/ 0);

        for (size_t i = 0; i < result.size(); i++)
        {
            result.value[i] = a.value[i] ^ b.value[i];
        }
        return result;
    }

    /**
     * @brief Compares two VeryLongInteger objects to determine if the left-hand side is greater than the right-hand side.
     *
     * This operator returns true if the result of subtracting rhs from lhs is positive,
     * indicating that lhs is greater than rhs.
     *
     * @param lhs The left-hand side VeryLongInteger to compare.
     * @param rhs The right-hand side VeryLongInteger to compare.
     * @return true if lhs is greater than rhs, false otherwise.
     */
    bool operator>(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsPositive();
    }

    /**
     * @brief Compares two VeryLongInteger objects for less-than relation.
     *
     * Determines if the left-hand side (lhs) VeryLongInteger is less than the right-hand side (rhs)
     * by subtracting lhs from rhs and checking if the result is positive.
     *
     * @param lhs The left-hand side VeryLongInteger to compare.
     * @param rhs The right-hand side VeryLongInteger to compare.
     * @return true if lhs is less than rhs, false otherwise.
     */
    bool operator<(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (rhs - lhs).IsPositive();
    }

    /**
     * @brief Compares two VeryLongInteger objects for greater than or equal to.
     *
     * Determines if the left-hand side (lhs) VeryLongInteger is greater than or equal to
     * the right-hand side (rhs) by subtracting rhs from lhs and checking if the result is non-negative.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return true if lhs is greater than or equal to rhs, false otherwise.
     */
    bool operator>=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsNonNegative();
    }

    /**
     * @brief Compares two VeryLongInteger objects for less than or equal to (<=).
     *
     * Determines if the left-hand side (lhs) VeryLongInteger is less than or equal to the right-hand side (rhs)
     * by subtracting lhs from rhs and checking if the result is non-negative.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return true if lhs is less than or equal to rhs, false otherwise.
     */
    bool operator<=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (rhs - lhs).IsNonNegative();
    }

    /**
     * @brief Equality operator for VeryLongInteger.
     *
     * Compares two VeryLongInteger objects for equality by subtracting one from the other
     * and checking if the result is zero.
     *
     * @param lhs The left-hand side VeryLongInteger operand.
     * @param rhs The right-hand side VeryLongInteger operand.
     * @return true if both operands are equal, false otherwise.
     */
    bool operator==(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return (lhs - rhs).IsZero();
    }

    /**
     * @brief Inequality operator for VeryLongInteger.
     *
     * Compares two VeryLongInteger objects for inequality.
     * Returns true if the objects are not equal, false otherwise.
     *
     * @param lhs The left-hand side VeryLongInteger.
     * @param rhs The right-hand side VeryLongInteger.
     * @return true if lhs and rhs are not equal, false otherwise.
     */
    bool operator!=(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * @brief Computes the power of a VeryLongInteger raised to a non-negative integer exponent.
     *
     * This function calculates @p val raised to the power of @p power (i.e., val^power).
     * The exponent must be non-negative; otherwise, an exception is thrown.
     *
     * @param val The base value as a VeryLongInteger.
     * @param power The non-negative integer exponent.
     * @return The result of val raised to the power of 'power'.
     * @throws std::invalid_argument if 'power' is negative.
     */
    inline VeryLongInteger Power(
        const VeryLongInteger& val,
        size_t power)
    {
        Requires::ArgumentNotNegative(power, NAMEOF(power), FUNCTION_INFO);
        VeryLongInteger result = VeryLongInteger{uint8_t(1)};
        for (size_t i = 0; i < power; i++)
            result = result * val;
        return result;
    }
}
