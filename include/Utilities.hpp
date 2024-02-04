#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <cstdlib>
#include <cstring>

#include "typedefs.h"

namespace Utilities
{
    template <typename T>
    inline T* allocate(uint64 length)
    {
        return (T*) malloc(length * sizeof(T));
    }

    template <typename T>
    inline void reallocate(T*& buffer, uint64 length)
    {
        buffer = (T*) realloc(buffer, length * sizeof(T));
    }

    template <typename T>
    inline void release(T* buffer)
    {
        free(buffer);
    }

    template <typename T>
    inline void move(T* destination, const T* source, uint64 length)
    {
        memcpy(destination, source, length * sizeof(T));
    }

    template <typename T>
    inline void move(T* values, uint64 length, uint64 offset)
    {
        memmove(values + offset, values, length * sizeof(T));
    }

    template <typename T>
    inline void copy(T* destination, const T* source, uint64 length)
    {
        memcpy(destination, source, length * sizeof(T));
    }


    template <typename T>
    inline T min(const T& left, const T& right)
    {
        return left < right ? left : right;
    }

    template <typename T>
    inline T max(const T& left, const T& right)
    {
        return left < right ? right : left;
    }
}

#endif // UTILITIES_HPP