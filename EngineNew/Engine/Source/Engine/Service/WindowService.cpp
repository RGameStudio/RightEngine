#include <Engine/Service/WindowService.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Registration.hpp>

RTTR_REGISTRATION
{
engine::registration::Service<engine::WindowService>("engine::WindowService");
}

namespace engine
{

WindowService::WindowService()
{
	ENGINE_ASSERT(glfwInit());

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(1920, 1080, "Right Engine", nullptr, nullptr);

	ENGINE_ASSERT(m_window);

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			Instance().Stop();
		});
}

WindowService::~WindowService()
{
	glfwDestroyWindow(m_window);
}

void WindowService::Update(float dt)
{
	glfwPollEvents();
}

void WindowService::PostUpdate(float dt)
{
	glfwSwapBuffers(m_window);
}

} // namespace engine