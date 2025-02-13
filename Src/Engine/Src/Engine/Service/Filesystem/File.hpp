#pragma once

#include <Engine/Service/Filesystem/IFilesystem.hpp>
#include <Core/Blob.hpp>

namespace engine::io
{

class ENGINE_API File
{
public:
	File(const fs::path& path);

	bool				Read(bool binary = true);

	const fs::path&		Path() const { return m_path; }
	size_t				Size() const { return m_data.size(); }
	void*				Raw() { return m_data.raw(); }
	std::string_view	AsStr() { return static_cast<const char*>(m_data.raw()); }

private:
	fs::path	m_path;
	core::Blob	m_data;
};

} // engine::io