#pragma once

#include "IService.hpp"
#include <taskflow/taskflow.hpp>

namespace RightEngine
{
	class ThreadService : public IService
	{
	public:
		template <typename F>
		auto AddBackgroundTask(F&& f);

		tf::Future<void> AddBackgroundTaskflow(tf::Taskflow&& taskflow);

		virtual void OnRegister() override;
		virtual void OnUpdate(float dt) override;

		virtual ~ThreadService() override;
	private:
		tf::Executor m_executor;
		std::list<tf::Taskflow> m_taskflows;
		std::mutex m_mutex;
	};

	template <typename F>
	auto ThreadService::AddBackgroundTask(F&& f)
	{
		return m_executor.async(f);
	}
}
