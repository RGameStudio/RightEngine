#pragma once

#include "AssetBase.hpp"
#include "Types.hpp"
#include "VertexBufferLayout.hpp"
#include "Device.hpp"
#include "ShaderProgramDescriptor.hpp"
#include <glm/matrix.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace RightEngine
{
    class Shader : public AssetBase
    {
    public:
        ASSET_BASE()

        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;
        virtual void SetUniform1ui(const std::string& name, uint32_t value) = 0;
        virtual void SetUniform1i(const std::string& name, int value) = 0;
        virtual void SetUniform1f(const std::string& name, float value) = 0;
        virtual void SetUniform2f(const std::string& name, float v0, float v1) = 0;
        virtual void SetUniform3f(const std::string& name, float v0, float v1, float v2) = 0;
        virtual void SetUniform3f(const std::string& name, const glm::vec3& vec3) = 0;
        virtual void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) = 0;
        virtual void SetUniform4f(const std::string& name, const glm::vec4& vec4) = 0;
        virtual void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) = 0;
        virtual void SetUniform1iv(const std::string& name, const std::vector<int>& v) = 0;

    protected:
        Shader(const std::shared_ptr<Device>& device, const ShaderProgramDescriptor& descriptor) : descriptor(descriptor)
        {}

        ShaderProgramDescriptor descriptor;
    };
}