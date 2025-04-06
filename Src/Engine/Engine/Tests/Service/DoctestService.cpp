#include <Engine/Tests/Service/DoctestService.hpp>
#include <Engine/Registration.hpp>
#include <Engine/Service/World/WorldService.hpp>
#include <Engine/Engine.hpp>
#include <doctest/doctest.h>

namespace tests
{

DoctestService::DoctestService()
{
    m_testThread = std::make_unique<engine::CustomThread>("Test thread");
}

DoctestService::~DoctestService()
{
}

void DoctestService::Update(float dt)
{
    
}

void DoctestService::PostUpdate(float dt)
{
    if (!m_waitFuture.valid())
    {
        m_waitFuture = m_testThread->AddTask([this]()
            {
                return m_context->run();
            });
    }

    if (m_waitFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready)
    {
        [[maybe_unused]] const auto status = m_waitFuture.get();
        ENGINE_ASSERT(status == 0);
        engine::Instance().Stop(status);
    }
}

} // tests
