#pragma once

#include "Shader.hpp"

namespace RightEngine
{
    struct ShaderProgramSource
    {
        std::string vertexSource;
        std::string fragmentSource;
    };

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
        virtual ~OpenGLShader() override;

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

    private:
        std::string filePath;
        uint32_t id;
        std::unordered_map<std::string, int> uniformLocationCache;

    private:
        ShaderProgramSource ParseShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
        uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
        uint32_t CompileShader(uint32_t type, const std::string& source);
        int GetUniformLocation(const std::string& name);
    };
}
