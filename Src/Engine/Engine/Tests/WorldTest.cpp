#include <Engine/Tests/Service/DoctestService.hpp>
#include <Engine/Service/WorldService.hpp>
#include <Engine/Engine.hpp>
#include <Engine/System/RenderSystem.hpp>

TEST_CASE("Simple entity manipulations")
{
    constexpr std::string_view C_TEST_ENTITY_NAME = "Test Entity";

    auto& ws = engine::Instance().Service<engine::WorldService>();
    auto& em = ws.CurrentWorld()->GetEntityManager();

    const auto uuid = em->CreateEntity(C_TEST_ENTITY_NAME);
    em->Update();
    const auto e = em->GetEntity(uuid);

    SUBCASE("Entity was created and component added")
    {
        em->AddComponent<engine::MeshComponent>(uuid, engine::MeshComponent());

        CHECK_EQ(em->GetEntityInfo(uuid).m_name, C_TEST_ENTITY_NAME);
        CHECK_NE(em->TryGetComponent<engine::MeshComponent>(uuid), nullptr);
        CHECK_NE(em->TryGetComponent<engine::MeshComponent>(e), nullptr);
    }

    SUBCASE("MeshComponent was deleted")
    {
        em->RemoveComponent<engine::MeshComponent>(uuid);

        CHECK_EQ(em->TryGetComponent<engine::MeshComponent>(uuid), nullptr);
    }

    SUBCASE("Entity was deleted")
    {
        em->RemoveEntity(uuid);
        em->Update();
    }
}
