#pragma once

#include "requires.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace cs
{
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
        uint8_t* value;

        // The number of bytes in the binary representation of the number.
        size_t nBytes;

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

    public:
        template <typename T>
        static VeryLongInteger FromInteger(T value)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            size_t nBytes = sizeof(T);
            VeryLongInteger result(nBytes);
            for (size_t i = 0; i < nBytes; ++i)
                result.value[i] = static_cast<uint8_t>(i >> (8 * i));
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
                size_t start = std::max(bitIndex - 8, 0ul);
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

        std::string ToDecimal() const;

        std::string ToHexadecimal() const
        {
            std::ostringstream oss;
            for (size_t i = 0; i < nBytes; ++i)
                oss << std::setw(2) << std::setfill('0') << std::hex << (int)value[nBytes - 1 - i];
            return oss.str();
        }

        std::string ToBinary() const
        {
            std::ostringstream oss;
            for (size_t i = 0; i < nBytes; ++i)
                oss << std::bitset<8>(value[nBytes - 1 - i]);
            std::string str = oss.str();
            return str;
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
        const VeryLongInteger& rhs);

    inline VeryLongInteger operator/(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    inline VeryLongInteger Power(
        const VeryLongInteger& val,
        size_t power)
    {
        Requires::ArgumentNotNegative(power, NAMEOF(power), FUNCTION_INFO);
        VeryLongInteger result = VeryLongInteger::FromInteger(1);
        for (size_t i = 0; i < power; ++i)
            result = result * val;
    }
}
