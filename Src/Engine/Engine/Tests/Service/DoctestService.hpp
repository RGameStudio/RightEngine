#pragma once

#include <Engine/Tests/Service/Config.hpp>
#include <Engine/Service/IService.hpp>
#include <Engine/Service/ThreadService.hpp>
#include <doctest/doctest.h>

namespace tests
{

class ENGINE_API DoctestService final : public engine::Service<DoctestService>
{
public:
    DoctestService();
    virtual ~DoctestService() override;

    virtual void Update(float dt) override;
    virtual void PostUpdate(float dt) override;

    void Context(std::unique_ptr<doctest::Context>&& context)
    {
        m_context = std::move(context);
    }

private:
    std::unique_ptr<doctest::Context> m_context;
    std::unique_ptr<engine::CustomThread> m_testThread;
    std::future<int> m_waitFuture;
};

} // tests