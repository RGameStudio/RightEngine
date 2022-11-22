#pragma once

#include <cstddef>
#include <functional>

namespace RightEngine
{
    class Utils
    {
    public:
        template<class T>
        static inline void CombineHash(std::size_t& seed, const T& value)
        {
            std::hash<T> hash;
            seed ^= hash(value) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
    };
}
