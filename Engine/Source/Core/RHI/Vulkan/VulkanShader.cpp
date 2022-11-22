#include "VulkanShader.hpp"
#include "Assert.hpp"
#include "Path.hpp"
#include "Logger.hpp"
#include "Device.hpp"
#include "VulkanDevice.hpp"
#include "VulkanConverters.hpp"
#include <StandAlone/ResourceLimits.h>
#include <vulkan/vulkan.h>
#include <fstream>
#include <sstream>

using namespace RightEngine;

namespace
{
    VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirv)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirv.size() * sizeof(uint32_t);
        createInfo.pCode = spirv.data();

        VkShaderModule shaderModule;

        auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(Device::Get());
        if (vkCreateShaderModule(vulkanDevice->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            R_CORE_ASSERT(false, "failed to create shader module!");
        }

        return shaderModule;
    }
}

VulkanShader::VulkanShader(const std::shared_ptr<Device>& device, const ShaderProgramDescriptor& descriptor) : Shader(device, descriptor)
{
    R_CORE_ASSERT(descriptor.layout.GetElements().size() > 0, "");
    ShaderDescriptor vertexShader;
    ShaderDescriptor fragmentShader;

    for (const auto& desc : descriptor.shaders)
    {
        switch (desc.type)
        {
            case ShaderType::VERTEX:
                vertexShader = desc;
                break;
            case ShaderType::FRAGMENT:
                fragmentShader = desc;
                break;
            default:
                R_CORE_ASSERT(false, "");
        }
    }

    const auto vertexShaderPath = vertexShader.path;
    const auto fragmentShaderPath = fragmentShader.path;
    glslang_initialize_process();
    ShaderProgramSource source = ParseShaders(vertexShaderPath, fragmentShaderPath);
    const auto vertSpirv = CompileShader(GLSLANG_STAGE_VERTEX, source.vertexSource.c_str(), vertexShaderPath.c_str());
    const auto fragSpirv = CompileShader(GLSLANG_STAGE_FRAGMENT, source.fragmentSource.c_str(), fragmentShaderPath.c_str());

    vertexShaderModule = CreateShaderModule(vertSpirv);
    fragShaderModule = CreateShaderModule(fragSpirv);
}

ShaderProgramSource VulkanShader::ParseShaders(const std::string& vertexShaderPath,
                                               const std::string& fragmentShaderPath)
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

    return { ss[0].str(), ss[1].str() };
}

std::vector<uint32_t> VulkanShader::CompileShader(glslang_stage_t stage, const char* shaderSource, const char* fileName)
{
    glslang_input_t input{};
    input.language = GLSLANG_SOURCE_GLSL;
    input.stage = stage;
    input.client = GLSLANG_CLIENT_VULKAN;
#ifdef R_APPLE
    input.client_version = GLSLANG_TARGET_VULKAN_1_0;
#else
    input.target_language_version = GLSLANG_TARGET_SPV_1_3;
#endif
    input.target_language = GLSLANG_TARGET_SPV;
#ifdef R_APPLE
    input.target_language_version = GLSLANG_TARGET_SPV_1_0;
#else
    input.target_language_version = GLSLANG_TARGET_SPV_1_3;
#endif
    input.code = shaderSource;
    input.default_version = 450;
    input.default_profile = GLSLANG_NO_PROFILE;
    input.force_default_version_and_profile = false;
    input.forward_compatible = false;
    input.messages = GLSLANG_MSG_DEFAULT_BIT;
    input.resource = reinterpret_cast<const glslang_resource_t*>(&glslang::DefaultTBuiltInResource);

    glslang_shader_t* shader = glslang_shader_create(&input);

    if (!glslang_shader_preprocess(shader, &input))
    {
        R_CORE_ERROR("Shader preprocessing failed. Shader name: {0}", fileName);
        R_CORE_ERROR("{0}", glslang_shader_get_info_log(shader));
        R_CORE_ERROR("{0}", glslang_shader_get_info_debug_log(shader));
//        R_CORE_ERROR("{0}", input.code);
        R_CORE_ASSERT(false, "");
        glslang_shader_delete(shader);
        return {};
    }

    if (!glslang_shader_parse(shader, &input))
    {
        R_CORE_ERROR("Shader parsing failed. Shader name: {0}", fileName);
        R_CORE_ERROR("{0}", glslang_shader_get_info_log(shader));
        R_CORE_ERROR("{0}", glslang_shader_get_info_debug_log(shader));
        R_CORE_ASSERT(false, "");
//        R_CORE_ERROR("{0}", glslang_shader_get_preprocessed_code(shader));
        glslang_shader_delete(shader);
        return {};
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        R_CORE_ERROR("GLSL linking failed {0}", fileName);
        R_CORE_ERROR("{0}", glslang_program_get_info_log(program));
        R_CORE_ERROR("{0}", glslang_program_get_info_debug_log(program));
        R_CORE_ASSERT(false, "");
        glslang_program_delete(program);
        glslang_shader_delete(shader);
        return {};
    }

    glslang_program_SPIRV_generate(program, stage);

    std::vector<uint32_t> outShaderModule(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, outShaderModule.data());

    const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
    if (spirv_messages)
    {
        R_CORE_INFO("[{0}]: {1}", fileName, spirv_messages);
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return outShaderModule;
}

VulkanShader::~VulkanShader()
{
    auto device = VK_DEVICE();
    vkDestroyShaderModule(device->GetDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(device->GetDevice(), vertexShaderModule, nullptr);
}

void VulkanShader::Bind() const
{

}

void VulkanShader::UnBind() const
{

}

VkVertexInputBindingDescription VulkanShader::GetVertexFormatDescription() const
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = descriptor.layout.GetStride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VulkanShader::GetVertexFormatAttribute() const
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    const auto layoutElements = descriptor.layout.GetElements();
    uint32_t offset = -layoutElements.front().GetSize();
    for (int i = 0; i < layoutElements.size(); i++)
    {
        VkVertexInputAttributeDescription attributeDescription;
        attributeDescription.binding = 0;
        attributeDescription.location = i;
        attributeDescription.format = VulkanConverters::Format(layoutElements[i].type);
        attributeDescription.offset = offset + layoutElements[std::max(0, i - 1)].GetSize();
        offset += layoutElements[i].GetSize();;

        attributeDescriptions.emplace_back(attributeDescription);
    }

    return attributeDescriptions;
}
