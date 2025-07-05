#include <Engine/Service/Resource/MaterialResource.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Registration.hpp>
#include <Engine/Service/ThreadService.hpp>
#include <Engine/Service/Filesystem/File.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <Engine/Service/Window/WindowService.hpp>
#include <Engine/Service/Resource/ResourceService.hpp>
#include <Engine/Service/Resource/TextureResource.hpp>
#include <Engine/Serialization/FromJson.hpp>
#include <RHI/Helpers.hpp>
#include <nlohmann/json.hpp>

#include "RHI/Pipeline.hpp"
#include "RHI/RenderPass.hpp"

RTTR_REGISTRATION
{
	using namespace engine::registration;

    ResourceLoader<engine::MaterialLoader>("engine::MaterialLoader")
        .Domain(engine::Domain::UI);

    // Регистрация структур для сериализации материала
    Class<engine::MaterialDependency>("engine::MaterialDependency")
        .Property("path", &engine::MaterialDependency::path)
        .Property("index", &engine::MaterialDependency::index)
        .Property("hasDependency", &engine::MaterialDependency::hasDependency);

    Class<engine::MaterialTextureSlot>("engine::MaterialTextureSlot")
        .Property("texturePath", &engine::MaterialTextureSlot::texturePath)
        .Property("slot", &engine::MaterialTextureSlot::slot)
        .Property("mipLevel", &engine::MaterialTextureSlot::mipLevel);

    Class<engine::MaterialAttachment>("engine::MaterialAttachment")
        .Property("loadOperation", &engine::MaterialAttachment::loadOperation)
        .Property("storeOperation", &engine::MaterialAttachment::storeOperation)
        .Property("dependency", &engine::MaterialAttachment::dependency);

    Class<engine::MaterialData>("engine::MaterialData")
        .Property("name", &engine::MaterialData::name)
        .Property("shader", &engine::MaterialData::shader)
        .Property("version", &engine::MaterialData::version)
        .Property("offscreen", &engine::MaterialData::offscreen)
        .Property("depthCompareOp", &engine::MaterialData::depthCompareOp)
        .Property("cullMode", &engine::MaterialData::cullMode)
        .Property("compute", &engine::MaterialData::compute)
        .Property("attachments", &engine::MaterialData::attachments)
        .Property("depthAttachment", &engine::MaterialData::depthAttachment)
        .Property("hasDepthAttachment", &engine::MaterialData::hasDepthAttachment)
        .Property("textureSlots", &engine::MaterialData::textureSlots);

    rttr::registration::enumeration<rhi::CullMode>("rhi::CullMode")
        (
	        rttr::value("NONE", rhi::CullMode::NONE),
	        rttr::value("BACK", rhi::CullMode::BACK),
	        rttr::value("FRONT", rhi::CullMode::FRONT)
	    );

	rttr::registration::enumeration<rhi::CompareOp>("rhi::CompareOp")
		(
			rttr::value("LESS", rhi::CompareOp::LESS),
			rttr::value("LESS_OR_EQUAL", rhi::CompareOp::LESS_OR_EQUAL),
			rttr::value("GREATER", rhi::CompareOp::GREATER)
		);

	rttr::registration::enumeration<rhi::AttachmentLoadOperation>("rhi::AttachmentLoadOperation")
		(
			rttr::value("UNDEFINED", rhi::AttachmentLoadOperation::UNDEFINED),
			rttr::value("LOAD", rhi::AttachmentLoadOperation::LOAD),
			rttr::value("CLEAR", rhi::AttachmentLoadOperation::CLEAR)
		);

	rttr::registration::enumeration<rhi::AttachmentStoreOperation>("rhi::AttachmentStoreOperation")
		(
			rttr::value("UNDEFINED", rhi::AttachmentStoreOperation::UNDEFINED),
			rttr::value("STORE", rhi::AttachmentStoreOperation::STORE)
		);
}

namespace
{

template<typename T>
T StringToEnum(const std::string& str)
{
	const auto type = rttr::type::get<T>();
	ENGINE_ASSERT(type.is_enumeration());

	const auto enumValue = type.get_enumeration();
	ENGINE_ASSERT(enumValue.is_valid());
	ENGINE_ASSERT(!enumValue.get_type().get_name().empty());

	return enumValue.name_to_value(std::string_view(str.data())).template get_value_unsafe<T>();
}

} // unnamed

namespace engine
{

MaterialLoader::MaterialLoader()
{
	m_shaderCompiler = Instance().Service<RenderService>().CreateShaderCompiler();
}

void MaterialLoader::Update()
{
	PROFILER_CPU_ZONE;
}

ResPtr<IResource> MaterialLoader::Load(const fs::path& path, bool immediate)
{
	std::lock_guard l(m_mutex);

	if (auto res = Get(path))
	{
		return res;
	}

	auto resource = MakeResPtr<MaterialResource>(path);
	resource->m_status = IResource::Status::LOADING;
	m_cache[path] = resource;

	auto load = [this, resource]
	{
		PROFILER_CPU_ZONE_NAME("Load material");
		const auto result = Load(resource);
		resource->m_status = result ? IResource::Status::READY : IResource::Status::FAILED;
	};

	if (immediate)
	{
		load();
		return resource;
	}

	auto& ts = Instance().Service<ThreadService>();
	ts.AddBackgroundTask([load]()
		{
			load();
		});

	return resource;
}

ResPtr<IResource> MaterialLoader::Get(const fs::path& path) const
{
	if (const auto it = m_cache.find(path); it != m_cache.end())
	{
		return it->second;
	}
	return {};
}

void MaterialLoader::LoadSystemResources()
{
	auto& rs = Instance().Service<ResourceService>();
	m_renderMaterial = std::static_pointer_cast<MaterialResource>(Load("/System/Materials/pbr.material"));
	m_presentMaterial = std::static_pointer_cast<MaterialResource>(Load("/System/Materials/present.material"));
	m_errorTexture = rs.Load<TextureResource>("/System/Textures/error.png");

	m_renderMaterial->Wait();
	m_presentMaterial->Wait();
	m_errorTexture->Wait();
}

const ResPtr<rhi::Pipeline>& MaterialLoader::Pipeline(const ResPtr<MaterialResource>& res) const
{
	std::lock_guard l(m_mutex);

	ENGINE_ASSERT(res);

	const auto it = m_shaderToPipeline.find(res->Material()->Shader());

	if (it == m_shaderToPipeline.end())
	{
		ENGINE_ASSERT(false);
		static ResPtr<rhi::Pipeline> empty;
		return empty;
	}

	const auto& p = it->second;

	return p;
}

void MaterialLoader::ResizePipelines(glm::ivec2 extent, bool offscreen)
{
	ENGINE_ASSERT(extent != glm::ivec2(0));

	eastl::vector<std::future<void>> tasks;

	for (const auto& cacheEntry : m_cache)
	{
		auto& resource = cacheEntry.second;

		// TODO: Probably we need to resize it later
		if (!resource->Ready())
		{
			continue;
		}

		const auto& pipeline = Pipeline(resource);

		if (pipeline->Descriptor().m_offscreen == offscreen && !pipeline->Descriptor().m_compute)
		{
			auto& ts = Instance().Service<ThreadService>();

			resource->m_status = IResource::Status::LOADING;

			tasks.push_back(ts.AddBackgroundTask([this, resource]()
				{
					PROFILER_CPU_ZONE_NAME("Load texture");
					const auto result = Load(resource, true);
					resource->m_status = result ? IResource::Status::READY : IResource::Status::FAILED;
				}));
		}
	}

	for (auto& task : tasks)
	{
		task.wait();
	}
}

bool MaterialLoader::Load(const ResPtr<MaterialResource>& resource, bool onlyResizePipeline)
{
	auto& vfs = Instance().Service<io::VirtualFilesystemService>();

	const auto& srcPath = resource->SourcePath();

	std::ifstream file(vfs.Absolute(srcPath));

	if (file.fail())
	{
		core::log::error("[MaterialLoader] Can't load material '{}'", resource->SourcePath().generic_u8string());
		return false;
	}

	// Читаем весь файл в строку
	std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Используем FromJson API для парсинга
	std::string errorBuffer;
	auto materialDataOpt = FromJsonString<MaterialData>(jsonContent, &errorBuffer);

	if (!materialDataOpt.has_value())
	{
		core::log::error("[MaterialLoader] Failed to parse material '{}': {}", resource->SourcePath().generic_u8string(), errorBuffer);
		return false;
	}

	auto materialData = materialDataOpt.value();

	if (materialData.name.empty())
	{
		core::log::error("[MaterialLoader] Material name is empty in '{}'", resource->SourcePath().generic_u8string());
		return false;
	}

	// Создаем ParsedPipelineInfo из MaterialData
	ParsedPipelineInfo parsedPipeline;
	parsedPipeline.m_compute = materialData.compute;
	parsedPipeline.m_offscreen = materialData.offscreen;
	parsedPipeline.m_depthCompareOp = StringToEnum<rhi::CompareOp>(materialData.depthCompareOp);
	parsedPipeline.m_cullMode = StringToEnum<rhi::CullMode>(materialData.cullMode);

	// Конвертируем attachment'ы
	for (const auto& attachmentData : materialData.attachments)
	{
		auto& attachment = parsedPipeline.m_attachments.emplace_back();
		attachment.m_descriptor.m_clearValue = {};
		attachment.m_descriptor.m_loadOperation = attachmentData.loadOperation;
		attachment.m_descriptor.m_storeOperation = attachmentData.storeOperation;

		if (attachmentData.dependency.hasDependency)
		{
			const auto& dep = attachmentData.dependency;
			attachment.m_dependency = dep.path;
			attachment.m_depAttachmentIndex = dep.index;
		}
	}

	// Конвертируем depth attachment
	if (materialData.hasDepthAttachment)
	{
		const auto& depthData = materialData.depthAttachment;
		LoadAttachmentDescriptor depthAttachment;
		depthAttachment.m_descriptor.m_clearValue = {};
		depthAttachment.m_descriptor.m_loadOperation = depthData.loadOperation;
		depthAttachment.m_descriptor.m_storeOperation = depthData.storeOperation;

		if (depthData.dependency.hasDependency)
		{
			const auto& dep = depthData.dependency;
			depthAttachment.m_dependency = dep.path;
			depthAttachment.m_depAttachmentIndex = dep.index;
		}

		parsedPipeline.m_depthAttachment = depthAttachment;
	}

	io::fs::path shaderPath = materialData.shader;

	std::shared_ptr<rhi::Shader> shader;

	{
		std::lock_guard l(m_mutex);
		if (const auto it = m_shaderCache.find(shaderPath); it != m_shaderCache.end())
		{
			shader = it->second;
		}
	}

	if (!shader)
	{
		auto& rs = Instance().Service<RenderService>();

		const auto shaderType = shaderPath.extension() == ".glsl" ? rhi::ShaderType::FX : rhi::ShaderType::COMPUTE;

		const auto shaderData = rs.RunOnRenderThreadWait([&]()
			{
				return m_shaderCompiler->Compile(vfs.Absolute(shaderPath).generic_u8string(), shaderType);
			});

		if (!shaderData.m_valid)
		{
			return false;
		}

		rhi::ShaderDescriptor desc;
		desc.m_name = materialData.name;
		desc.m_type = shaderType;
		desc.m_reflection = shaderData.m_reflection;
		desc.m_blobByStage = shaderData.m_stageBlob;
		desc.m_path = shaderPath.generic_u8string();

		shader = rs.CreateShader(desc);

		{
			std::lock_guard l(m_mutex);
			m_shaderCache[shaderPath] = shader;
		}
	}

	parsedPipeline.m_shader = shader;

	bool hasPipeline = false;

	{
		std::lock_guard l(m_mutex);
		if (const auto it = m_shaderToPipeline.find(shader); it != m_shaderToPipeline.end())
		{
			hasPipeline = true;
		}
	}

	if (!hasPipeline || onlyResizePipeline)
	{
		parsedPipeline.m_viewportSize = parsedPipeline.m_offscreen ? 
			Instance().Service<RenderService>().ViewportSize() :
			Instance().Service<WindowService>().FramebufferExtent();

		if (parsedPipeline.m_viewportSize.x < 1 ||
			parsedPipeline.m_viewportSize.y < 1 ||
			parsedPipeline.m_viewportSize.x > 65536 ||
			parsedPipeline.m_viewportSize.y > 65536)
		{
			core::log::warning("[MaterialLoader] Viewport size is out of bounds: {}x{}", parsedPipeline.m_viewportSize.x,
				parsedPipeline.m_viewportSize.y);
		}

		parsedPipeline.m_viewportSize = glm::clamp(parsedPipeline.m_viewportSize, glm::ivec2(1, 1), glm::ivec2(65536, 65536));

		m_shaderToPipeline[shader] = AllocatePipeline(parsedPipeline);
	}

	if (!onlyResizePipeline)
	{
		resource->m_material = std::make_shared<render::Material>(shader);
	}

	for (const auto& [slot, buffer] : shader->Descriptor().m_reflection.m_bufferMap)
	{
		rttr::type type = rttr::type::get_by_name(fmt::format("engine::{}", buffer.m_name));
		resource->m_material->SetBuffer(type, slot, buffer.m_stage, buffer.m_name);
	}

	auto& resourceService = Instance().Service<ResourceService>();
	for (const auto& textureSlot : materialData.textureSlots)
	{
		if (!textureSlot.texturePath.empty())
		{
			auto textureResource = resourceService.Load<TextureResource>(textureSlot.texturePath, true);
			textureResource->Wait();

			if (textureResource->Ready())
			{
				resource->m_material->SetTexture(textureResource->Texture(), textureSlot.slot, textureSlot.mipLevel);
			}
			else
			{
				core::log::warning("[MaterialLoader] Failed to load texture '{}' for material '{}'",
					textureSlot.texturePath.generic_u8string(), materialData.name);
			}
		}
	}

	resource->m_material->Sync();
	return true;
}

MaterialData MaterialLoader::ParseMaterialData(const std::string& jsonContent)
{
	std::string errorBuffer;
	auto materialDataOpt = FromJsonString<MaterialData>(jsonContent, &errorBuffer);

	if (!materialDataOpt.has_value())
	{
		core::log::error("[MaterialLoader] Failed to parse material data: {}", errorBuffer);
		return {};
	}

	return materialDataOpt.value();
}

std::shared_ptr<rhi::Pipeline> MaterialLoader::AllocatePipeline(ParsedPipelineInfo& info)
{
	auto& rs = Instance().Service<RenderService>();

	if (info.m_compute)
	{
		const auto computePass = std::make_shared<rhi::ComputePass>();

		rhi::PipelineDescriptor computePipelineDesc{};
		computePipelineDesc.m_compute = true;
		computePipelineDesc.m_computePass = computePass;
		computePipelineDesc.m_shader = info.m_shader;

		return rs.CreatePipeline(computePipelineDesc);
	}

	ENGINE_ASSERT(info.m_viewportSize != glm::ivec2(0));

	eastl::vector<rhi::AttachmentDescriptor> colorAttachments;

	for (auto& attachment : info.m_attachments)
	{
		if (attachment.m_dependency.empty())
		{
			rhi::TextureDescriptor colorAttachmentDesc;
			colorAttachmentDesc.m_type = rhi::TextureType::TEXTURE_2D;
			colorAttachmentDesc.m_format = rhi::Format::BGRA8_UNORM;
			colorAttachmentDesc.m_width = static_cast<uint16_t>(info.m_viewportSize.x);
			colorAttachmentDesc.m_height = static_cast<uint16_t>(info.m_viewportSize.y);
			colorAttachmentDesc.m_layersAmount = 1;

			attachment.m_descriptor.m_texture = rs.CreateTexture(colorAttachmentDesc);
		}
		else
		{
			// TODO: Improve logging by adding info for the waiting object
			core::log::debug("[MaterialLoader] Waiting for dependency: '{}'", attachment.m_dependency.generic_u8string());
			auto dependency = std::static_pointer_cast<MaterialResource>(Load(attachment.m_dependency));

			dependency->Wait();

			auto depPipeline = rs.Pipeline(dependency);

			attachment.m_descriptor.m_texture = depPipeline->Descriptor().m_pass->Descriptor().m_colorAttachments[attachment.m_depAttachmentIndex].m_texture;
		}
	}

	if (info.m_depthAttachment)
	{
		auto& depth = *info.m_depthAttachment;
		if (depth.m_dependency.empty())
		{
			rhi::TextureDescriptor depthDesc;
			depthDesc.m_type = rhi::TextureType::TEXTURE_2D;
			depthDesc.m_format = rhi::Format::D32_SFLOAT_S8_UINT;
			depthDesc.m_width = static_cast<uint16_t>(info.m_viewportSize.x);
			depthDesc.m_height = static_cast<uint16_t>(info.m_viewportSize.y);
			depthDesc.m_layersAmount = 1;

			depth.m_descriptor.m_texture = rs.CreateTexture(depthDesc);
		}
		else
		{
			// TODO: Improve logging by adding info for the waiting object
			core::log::debug("[MaterialLoader] Waiting for dependency: '{}'", depth.m_dependency.generic_u8string());
			auto dependency = std::static_pointer_cast<MaterialResource>(Load(depth.m_dependency));

			dependency->Wait();

			auto depPipeline = rs.Pipeline(dependency);

			depth.m_descriptor.m_texture = depPipeline->Descriptor().m_pass->Descriptor().m_depthStencilAttachment.m_texture;
		}
	}

	rhi::RenderPassDescriptor renderPassDesc{};
	renderPassDesc.m_name = fmt::format("{}-Pass", info.m_shader->Descriptor().m_name);
	renderPassDesc.m_extent = info.m_viewportSize;

	for (auto& attachment : info.m_attachments)
	{
		renderPassDesc.m_colorAttachments.emplace_back(std::move(attachment.m_descriptor));
	}

	renderPassDesc.m_depthStencilAttachment = info.m_depthAttachment.has_value() ? info.m_depthAttachment->m_descriptor : rhi::AttachmentDescriptor{};

	const auto renderpass = rs.CreateRenderPass(renderPassDesc);

	rhi::PipelineDescriptor pipelineDesc{};
	pipelineDesc.m_compute = false;
	pipelineDesc.m_cullMode = info.m_cullMode;
	pipelineDesc.m_depthCompareOp = info.m_depthCompareOp;
	pipelineDesc.m_offscreen = info.m_offscreen;
	pipelineDesc.m_pass = renderpass;
	pipelineDesc.m_shader = info.m_shader;

	const auto pipeline = rs.CreatePipeline(pipelineDesc);
	return pipeline;
}

MaterialResource::MaterialResource(const io::fs::path& path) : Resource(path)
{
}

} // engine