#include <gtest/gtest.h>

#include "ats/world/World.hpp"
#include "ats/world/Tree.hpp"
#include "ats/world/Transform2D.hpp"

TEST(TreeTests, TreeEntityStoresGeometryAndPosition)
{
    ats::world::World world;

    const entt::entity tree = world.CreateEntity();

    world.GetRegistry().emplace<ats::world::Tree>(tree);

    world.GetRegistry().emplace<ats::world::Transform2D>(
        tree,
        ats::math::Vector2{ 100.0, 200.0 },
        0.0
    );

    const auto& registry = world.GetRegistry();

    const bool hasComponents =
        registry.all_of<ats::world::Tree, ats::world::Transform2D>(tree);

    ASSERT_TRUE(hasComponents);

    const auto& geometry = registry.get<ats::world::Tree>(tree);
    const auto& transform = registry.get<ats::world::Transform2D>(tree);

    EXPECT_DOUBLE_EQ(geometry.trunkRadius, 15.0);
    EXPECT_DOUBLE_EQ(geometry.canopyRadius, 35.0);

    EXPECT_DOUBLE_EQ(transform.position.x, 100.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 200.0);
}