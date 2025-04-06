#pragma once

#include <Engine/Config.hpp>
#include <Engine/Service/Filesystem/VirtualFilesystemService.hpp>
#include <Core/Type.hpp>

namespace engine
{

class ENGINE_API IResource : core::NonCopyable
{
	RTTR_DECLARE_ROOT()
	RTTR_ENABLE_OBJECT_INFO()

public:
	enum class Status : uint8_t
	{
		UNKNOWN = 0,
		LOADING = 1,
		FAILED = 2,
		READY = 3
	};

	IResource(const io::fs::path& path);
	virtual ~IResource() {}

	const io::fs::path&		SourcePath() const { return m_srcPath; }
	bool					Ready() const { return m_status == Status::READY; }
	void					Wait() const { while (!Ready()) { std::this_thread::yield(); } }

	io::fs::path	m_srcPath;
protected:
	Status			m_status = Status::UNKNOWN;
};

template<typename T>
class Resource : public IResource
{
	RTTR_DECLARE_ANCESTORS(IResource)
	RTTR_ENABLE_OBJECT_INFO_AS(T)

public:
	explicit Resource(const io::fs::path& path)
		: IResource(path)
	{
	}
};

} // engine