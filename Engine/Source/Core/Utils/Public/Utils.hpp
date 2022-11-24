#pragma once

#include <glm/glm.hpp>
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

        static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
    };
}
