#include <gtest/gtest.h>

#include "ats/world/Transform2D.hpp"
#include "ats/world/World.hpp"

using namespace ats::world;

TEST(WorldTests, NewWorldHasNoEntities)
{
    ats::world::World world;

    EXPECT_EQ(world.EntityCount(), 0);
}

TEST(WorldTests, CreateEntityIncreasesCount)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    EXPECT_TRUE(world.IsValidEntity(entity));
    EXPECT_EQ(world.EntityCount(), 1);
}

TEST(WorldTests, DestroyEntityDecreasesCount)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    world.DestroyEntity(entity);

    EXPECT_FALSE(world.IsValidEntity(entity));
    EXPECT_EQ(world.EntityCount(), 0);
}

TEST(WorldTests, MultipleEntitiesExistIndependently)
{
    ats::world::World world;

    const entt::entity first = world.CreateEntity();
    const entt::entity second = world.CreateEntity();
    const entt::entity third = world.CreateEntity();

    EXPECT_EQ(world.EntityCount(), 3);

    world.DestroyEntity(second);

    EXPECT_TRUE(world.IsValidEntity(first));
    EXPECT_FALSE(world.IsValidEntity(second));
    EXPECT_TRUE(world.IsValidEntity(third));

    EXPECT_EQ(world.EntityCount(), 2);
}

TEST(WorldTests, DestroyedEntityIsInvalid)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    EXPECT_TRUE(world.IsValidEntity(entity));

    world.DestroyEntity(entity);

    EXPECT_FALSE(world.IsValidEntity(entity));
}


TEST(WorldTests, EntityCanHaveTransformComponent)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::Transform2D>(entity);

    EXPECT_TRUE(registry.all_of<ats::world::Transform2D>(entity));
}

TEST(WorldTests, TransformComponentStoresIndependentData)
{
    ats::world::World world;

    const entt::entity first = world.CreateEntity();
    const entt::entity second = world.CreateEntity();

    auto& registry = world.GetRegistry();

    auto& firstTransform = registry.emplace<ats::world::Transform2D>(first);
    auto& secondTransform = registry.emplace<ats::world::Transform2D>(second);

    firstTransform.position.x = 10.0;
    secondTransform.position.x = 25.0;

    EXPECT_DOUBLE_EQ(
        registry.get<ats::world::Transform2D>(first).position.x,
        10.0
    );

    EXPECT_DOUBLE_EQ(
        registry.get<ats::world::Transform2D>(second).position.x,
        25.0
    );
}

TEST(WorldTests, ComponentCanBeRemovedWithoutDestroyingEntity)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::Transform2D>(entity);

    registry.remove<ats::world::Transform2D>(entity);

    EXPECT_TRUE(world.IsValidEntity(entity));
    EXPECT_FALSE(registry.all_of<ats::world::Transform2D>(entity));
    EXPECT_EQ(world.EntityCount(), 1);
}

TEST(WorldTests, DestroyingEntityRemovesItsComponents)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::Transform2D>(entity);

    world.DestroyEntity(entity);

    EXPECT_FALSE(world.IsValidEntity(entity));
    EXPECT_EQ(world.EntityCount(), 0);
    EXPECT_EQ(registry.storage<ats::world::Transform2D>().size(), 0);
}
