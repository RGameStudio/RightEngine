#pragma once

#include <Engine/Config.hpp>

namespace engine::editor
{

class ENGINE_API Panel
{
public:
	Panel(std::string_view name, bool menuBar = false) : m_name(name), m_menuBar(menuBar)
	{}

	virtual ~Panel() = default;

	virtual void Draw();

	bool IsHovered() const
	{
		return m_isHovered;
	}

protected:
	virtual void DrawPanel() = 0;

	std::string m_name;
	bool		m_isHovered = false;
	bool		m_menuBar = false;
};

} // engine::editor