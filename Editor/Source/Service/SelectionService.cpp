#include "SelectionService.hpp"

using namespace editor;

void SelectionService::OnRegister()
{}

void SelectionService::OnUpdate(float dt)
{}

SelectionService::~SelectionService()
{}

void SelectionService::Entity(const std::shared_ptr<RightEngine::Entity>& entity)
{
	m_entity = entity;
	if (entity && m_selectEntityCallback)
	{
		m_selectEntityCallback(entity);
	}
	else if (!entity && m_deselectEntityCallback)
	{
		m_deselectEntityCallback();
	}
}

std::shared_ptr<RightEngine::Entity> SelectionService::Entity() const
{
	if (m_entity.expired())
	{
		return nullptr;
	}
	return m_entity.lock();
}
