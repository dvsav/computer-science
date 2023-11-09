#pragma once

#include "conditional_compilation.h" // for USE_REQUIRES

#include <string>    // for std::string
#include <stdexcept> // for std::invalid_argument, std::runtime_error

namespace Requires
{
    template<class TValue>
    static inline void ArgumentPositive(
        const TValue value,
        const std::string& argument_name,
        const std::string& function)
    {
        #ifdef USE_REQUIRES
        if (value <= 0)
            throw std::invalid_argument(function + ":\n    " + argument_name + " should be positive");
        #endif
    }

    template<class TValue>
    static inline void ArgumentNotNegative(
        const TValue value,
        const std::string& argument_name,
        const std::string& function)
    {
        #ifdef USE_REQUIRES
        if (value < 0)
            throw std::invalid_argument(function + ":\n    " + argument_name + " should not be negative");
        #endif
    }

    template<class TValue>
    static inline void ArgumentNotNull(
        TValue* pvalue,
        const std::string& argument_name,
        const std::string& function)
    {
        #ifdef USE_REQUIRES
        if (pvalue == nullptr)
            throw std::invalid_argument(function + ":\n    " + argument_name + " should not be NULL");
        #endif
    }

    static inline void That(
        bool condition,
        const std::string& function)
    {
        #ifdef USE_REQUIRES
        if (!condition)
            throw std::runtime_error(function);
        #endif
    }

    static inline void That(
        bool condition,
        const std::string& message,
        const std::string& function)
    {
        #ifdef USE_REQUIRES
        if (!condition)
            throw std::runtime_error(function + ":\n    " + message);
        #endif
    }
}

#define FUNCTION_INFO (std::string(__FILE__) + "\nline:" + std::to_string(__LINE__) + " " + std::string(__FUNCTION__))
#define NAMEOF(name) #name
