#include <Engine/Tests/Service/DoctestService.hpp>
#include <Engine/Service/WorldService.hpp>
#include <Engine/Engine.hpp>
#include <Engine/System/RenderSystem.hpp>

TEST_CASE("Create entity and add component")
{
    constexpr std::string_view C_TEST_ENTITY_NAME = "Test Entity";

    auto& ws = engine::Instance().Service<engine::WorldService>();
    auto& em = ws.CurrentWorld()->GetEntityManager();

    const auto uuid = em->CreateEntity(C_TEST_ENTITY_NAME);
    em->Update();

    em->AddComponent<engine::MeshComponent>(uuid, engine::MeshComponent());

    CHECK_NE(em->TryGetComponent<engine::MeshComponent>(uuid), nullptr);
    CHECK_EQ(em->GetEntityInfo(uuid).m_name, C_TEST_ENTITY_NAME);
}