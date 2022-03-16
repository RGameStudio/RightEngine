#pragma once

#include "SceneNode.hpp"

namespace RightEngine
{
    enum class LightNodeType
    {
        AMBIENT,
        POINT_LIGHT
    };

    class LightNode: public SceneNode
    {
    public:
        explicit LightNode(LightNodeType type);

        const glm::vec3& GetColor() const;
        void SetColor(const glm::vec3& newColor);

        float GetIntensity() const;
        void SetIntensity(float newIntensity);

        LightNodeType GetType() const;

    private:
        LightNodeType type{ LightNodeType::POINT_LIGHT};
        glm::vec3 color{ 1.0f, 1.0f, 1.0f };
        float intensity{ 1.0f };
    };
}
