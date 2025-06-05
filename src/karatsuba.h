#pragma once

#include "requires.h"

#include <algorithm>
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
        unsigned char* value;

        // The number of bytes in the binary representation of the number.
        size_t nBytes;

        /**
         * @brief Creates a VeryLongInteger and initializes it to 0.
         * @param size - the number of bytes in the binary representation of the number.
         */
        VeryLongInteger(size_t size) :
            nBytes(size),
            value(new unsigned char[nBytes])
        {
            for (size_t i = 0; i < nBytes; i++)
                value[i] = 0;
        }

    public:
        /**
         * @brief Copy constructor
         */
        VeryLongInteger(const VeryLongInteger& other) :
            nBytes(other.nBytes),
            value(new unsigned char[other.nBytes])
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
                    value = new unsigned char[nBytes];
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
        static VeryLongInteger FromDecimal(const std::string& decimal)
        {

        }

        static VeryLongInteger FromHexadecimal(const std::string& hex)
        {
            // number with removed prefix "0x" or "OX"
            std::string cleaned = hex;
            if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0)
                cleaned = hex.substr(2);

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
                result.value[i] = static_cast<unsigned char>(
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
                result.value[i] = static_cast<unsigned char>(
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

        std::string ToDecimal() const;

        std::string ToHexadecimal() const;

        std::string ToBinary() const;

    public:
        friend VeryLongInteger operator+(
            const VeryLongInteger& lhs,
            const VeryLongInteger& rhs);
        {
            size_t maxSize = std::max(lhs.size(), rhs.size())
            VeryLongInteger a = lhs.Extend(maxSize);
            VeryLongInteger b = rhs.Extend(maxSize);

            for (size_t i = 0; i < maxSize; ++i)
            {
                
            }
        }

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
        const VeryLongInteger& rhs);

    inline VeryLongInteger operator-(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    inline VeryLongInteger operator*(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    inline VeryLongInteger operator/(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);
}
