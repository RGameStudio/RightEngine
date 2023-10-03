#pragma once

#include <RHI/Config.hpp>
#include <RHI/SamplerDescriptor.hpp>
#include <RHI/ShaderCompiler.hpp>
#include <RHI/IContext.hpp>
#include <Core/Type.hpp>

namespace rhi
{
	class Buffer;
	class Shader;
	class Sampler;

	class RHI_API Device : public core::NonCopyable
	{
	public:
		virtual ~Device() = default;

		virtual std::shared_ptr<ShaderCompiler> CreateShaderCompiler(const ShaderCompiler::Options& options = {}) = 0;
		virtual std::shared_ptr<Buffer>			CreateBuffer(const BufferDescriptor& desc, const void* data) = 0;
		virtual std::shared_ptr<Shader>			CreateShader(const ShaderDescriptor& desc) = 0;
		virtual std::shared_ptr<Sampler>		CreateSampler(const SamplerDescriptor& desc) = 0;

		static std::shared_ptr<Device>			Create(const std::shared_ptr<IContext>& ctx);
	};
}