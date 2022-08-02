#pragma once

#include "Shader.hpp"
#include <glslang/Include/glslang_c_interface.h>
#include <vulkan/vulkan.h>

namespace RightEngine
{
    enum class ShaderModuleType
    {
        VERTEX,
        FRAGMENT
    };
    struct ShaderProgramSource
    {
        std::string vertexSource;
        std::string fragmentSource;
    };

    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::string& vertexShaderPath,
                     const std::string& fragmentShaderPath);
        virtual ~VulkanShader() override;

        virtual void Bind() const override;
        virtual void UnBind() const override;
        virtual void SetUniform1ui(const std::string& name, uint32_t value) override;
        virtual void SetUniform1i(const std::string& name, int value) override;
        virtual void SetUniform1f(const std::string& name, float value) override;
        virtual void SetUniform2f(const std::string& name, float v0, float v1) override;
        virtual void SetUniform3f(const std::string& name, float v0, float v1, float v2) override;
        virtual void SetUniform3f(const std::string& name, const glm::vec3& vec3) override;
        virtual void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) override;
        virtual void SetUniform4f(const std::string& name, const glm::vec4& vec4) override;
        virtual void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) override;
        virtual void SetUniform1iv(const std::string& name, const std::vector<int>& v) override;

        const VkShaderModule& GetShaderModule(ShaderModuleType type) const
        {
            switch (type)
            {
                case ShaderModuleType::VERTEX:
                    return vertexShaderModule;
                case ShaderModuleType::FRAGMENT:
                    return fragShaderModule;
            }
        }

    private:
        VkShaderModule vertexShaderModule;
        VkShaderModule fragShaderModule;

        ShaderProgramSource ParseShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

        std::vector<uint32_t> CompileShader(glslang_stage_t stage, const char* shaderSource, const char* fileName);
    };
}