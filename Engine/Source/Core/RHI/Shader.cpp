#include "Shader.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>

//TODO Error handling

RightEngine::Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    ShaderProgramSource source = ParseShaders(vertexShaderPath, fragmentShaderPath);
    id = CreateShader(source.vertexSource, source.fragmentSource);
}

RightEngine::Shader::~Shader()
{
    glDeleteProgram(id);
}

void RightEngine::Shader::Bind() const
{
    glUseProgram(id);
}

void RightEngine::Shader::UnBind() const
{
    glUseProgram(0);
}

void RightEngine::Shader::SetUniform1ui(const std::string& name, uint32_t value)
{
    glUniform1ui(GetUniformLocation(name), value);
}

void RightEngine::Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void RightEngine::Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void RightEngine::Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

void RightEngine::Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void RightEngine::Shader::SetUniform3f(const std::string& name, const glm::vec3& vec3)
{
    SetUniform3f(name, vec3.x, vec3.y, vec3.z);
}

void RightEngine::Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void RightEngine::Shader::SetUniform4f(const std::string& name, const glm::vec4& vec4)
{
    SetUniform4f(name, vec4.x, vec4.y, vec4.z, vec4.w);
}

void RightEngine::Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

RightEngine::ShaderProgramSource
RightEngine::Shader::ParseShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::ifstream vertexShaderStream(Path::ConvertEnginePathToOSPath(vertexShaderPath));
    std::ifstream fragmentShaderStream(Path::ConvertEnginePathToOSPath(fragmentShaderPath));

    if (!vertexShaderStream.is_open())
    {
        R_CORE_ERROR("Can't open vertex shader at path {0}", vertexShaderPath);
    }

    if (!fragmentShaderStream.is_open())
    {
        R_CORE_ERROR("Can't open fragment shader at path {0}", fragmentShaderPath);
    }

    std::string line;
    std::stringstream ss[2];

    while (std::getline(vertexShaderStream, line))
    {
        ss[0] << line << '\n';
    }

    while (std::getline(fragmentShaderStream, line))
    {
        ss[1] << line << '\n';
    }

    return {ss[0].str(), ss[1].str()};
}

uint32_t RightEngine::Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    uint32_t program = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

uint32_t RightEngine::Shader::CompileShader(uint32_t type, const std::string& source)
{
    uint32_t shaderId = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::string shaderType = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        R_CORE_ERROR("Failed to compile {0} shader!", shaderType);
        R_CORE_ERROR("Error message: {0}", message);
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

int RightEngine::Shader::GetUniformLocation(const std::string& name)
{
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];
    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1)
        R_CORE_WARN("Uniform {0} doesn't exist!", name);
    uniformLocationCache[name] = location;
    return location;
}
