#include <gtest/gtest.h>

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/DamageResetSystem.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

TEST(DamageResetSystemTests, ResetsAccumulatedDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 15.0;
    accumulator.externalDamage = 25.0;

    ats::world::DamageResetSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(DamageResetSystemTests, DoesNotModifySurvivalState)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state =
        registry.emplace<ats::world::SurvivalState>(entity);

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    state.food = 40.0;
    state.water = 60.0;
    state.health = 75.0;

    accumulator.internalDamage = 10.0;
    accumulator.externalDamage = 20.0;

    ats::world::DamageResetSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.food, 40.0);
    EXPECT_DOUBLE_EQ(state.water, 60.0);
    EXPECT_DOUBLE_EQ(state.health, 75.0);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(DamageResetSystemTests, ResetsMultipleEntities)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    auto& damageA =
        registry.emplace<ats::world::DamageAccumulator>(entityA);

    auto& damageB =
        registry.emplace<ats::world::DamageAccumulator>(entityB);

    damageA.internalDamage = 5.0;
    damageA.externalDamage = 10.0;

    damageB.internalDamage = 20.0;
    damageB.externalDamage = 30.0;

    ats::world::DamageResetSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(damageA.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(damageA.externalDamage, 0.0);

    EXPECT_DOUBLE_EQ(damageB.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(damageB.externalDamage, 0.0);
}

TEST(DamageResetSystemTests, HandlesEmptyWorld)
{
    ats::world::World world;
    ats::world::DamageResetSystem system;

    EXPECT_NO_THROW(system.Update(world));
}

