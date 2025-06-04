#pragma once

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
        size_t size;

        /**
         * @brief Creates a VeryLongInteger and initializes it to 0.
         * @param size - the number of bytes in the binary representation of the number.
         */
        VeryLongInteger(size_t size) :
            size(size),
            value(new unsigned char[size])
        {
            for (size_t i = 0; i < size; i++)
                value[i] = 0;
        }

    public:
        /**
         * @brief Copy constructor
         */
        VeryLongInteger(const VeryLongInteger& other) :
            size(other.size),
            value(new unsigned char[other.size])
        {
            std::copy(other.value, other.value + size, value);
        }

        /**
         * @brief Move constructor
         */
        VeryLongInteger(VeryLongInteger&& other) noexcept :
            size(other.size),
            value(other.value)
        {
            other.value = nullptr;
            other.size = 0;
        }

        ~VeryLongInteger()
        {
            if (value)
            {
                delete[] value;
                value = nullptr;
                size = 0;
            }
        }

        /**
         * @brief Copy assignment operator
         */
        VeryLongInteger& operator=(const VeryLongInteger& other)
        {
            if (this != &other)
            {
                delete[] value;
                size = other.size;
                value = new unsigned char[size];
                std::copy(other.value, other.value + size, value);
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
                size = other.size;
                other.value = nullptr;
                other.size = 0;
            }
            return *this;
        }

    public:
        static VeryLongInteger FromDecimal(const std::string& decimal);

        static VeryLongInteger FromHexadecimal(const std::string& hex);

        static VeryLongInteger FromBinary(const std::string& bin);

    public:
        std::string ToDecimal() const;

        std::string ToHexadecimal() const;

        std::string ToBinary() const;

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

    VeryLongInteger operator+(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    VeryLongInteger operator-(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    VeryLongInteger operator*(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);

    VeryLongInteger operator/(
        const VeryLongInteger& lhs,
        const VeryLongInteger& rhs);
}
