#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/HealthApplicationSystem.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

TEST(HealthApplicationSystemTests, InternalDamageReducesHealth)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 25.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.health, 75.0);
    EXPECT_DOUBLE_EQ(state.food, 100.0);
    EXPECT_DOUBLE_EQ(state.water, 100.0);
}

TEST(HealthApplicationSystemTests, ExternalDamageReducesHealth)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.externalDamage = 40.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.health, 60.0);
}

TEST(HealthApplicationSystemTests, InternalAndExternalDamageStack)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 15.0;
    accumulator.externalDamage = 20.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.health, 65.0);
}

TEST(HealthApplicationSystemTests, HealthIsClampedToZero)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    state.health = 10.0;

    accumulator.internalDamage = 50.0;
    accumulator.externalDamage = 75.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.health, 0.0);
}

TEST(HealthApplicationSystemTests, ZeroDamageDoesNotChangeHealth)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    registry.emplace<ats::world::DamageAccumulator>(entity);

    state.health = 72.5;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(state.health, 72.5);
}

TEST(HealthApplicationSystemTests, DoesNotModifyDamageAccumulator)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    registry.emplace<ats::world::SurvivalState>(entity);

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 5.0;
    accumulator.externalDamage = 12.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 5.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 12.0);
}

TEST(HealthApplicationSystemTests, MultipleEntitiesReceiveIndependentDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    auto& stateA = registry.emplace<ats::world::SurvivalState>(entityA);
    auto& stateB = registry.emplace<ats::world::SurvivalState>(entityB);

    auto& damageA = registry.emplace<ats::world::DamageAccumulator>(entityA);
    auto& damageB = registry.emplace<ats::world::DamageAccumulator>(entityB);

    stateB.health = 80.0;

    damageA.internalDamage = 10.0;
    damageB.externalDamage = 25.0;

    ats::world::HealthApplicationSystem system;
    system.Update(world);

    EXPECT_DOUBLE_EQ(stateA.health, 90.0);
    EXPECT_DOUBLE_EQ(stateB.health, 55.0);
}

TEST(HealthApplicationSystemTests, IgnoresEntitiesMissingRequiredComponents)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto survivalOnly = world.CreateEntity();
    const auto accumulatorOnly = world.CreateEntity();

    auto& state =
        registry.emplace<ats::world::SurvivalState>(survivalOnly);

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(accumulatorOnly);

    accumulator.internalDamage = 20.0;

    ats::world::HealthApplicationSystem system;

    EXPECT_NO_THROW(system.Update(world));

    EXPECT_DOUBLE_EQ(state.health, 100.0);
    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 20.0);
}

TEST(HealthApplicationSystemTests, RejectsNegativeDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = -5.0;

    ats::world::HealthApplicationSystem system;

    EXPECT_THROW(system.Update(world), std::invalid_argument);
    EXPECT_DOUBLE_EQ(state.health, 100.0);

    accumulator.internalDamage = 0.0;
    accumulator.externalDamage = -5.0;

    EXPECT_THROW(system.Update(world), std::invalid_argument);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(HealthApplicationSystemTests, RejectsNonFiniteDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::HealthApplicationSystem system;

    accumulator.internalDamage =
        std::numeric_limits<double>::infinity();

    EXPECT_THROW(system.Update(world), std::invalid_argument);
    EXPECT_DOUBLE_EQ(state.health, 100.0);

    accumulator.internalDamage = 0.0;
    accumulator.externalDamage =
        std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(system.Update(world), std::invalid_argument);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(HealthApplicationSystemTests, RejectsOverflowingTotalDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage =
        std::numeric_limits<double>::max();

    accumulator.externalDamage =
        std::numeric_limits<double>::max();

    ats::world::HealthApplicationSystem system;

    EXPECT_THROW(system.Update(world), std::overflow_error);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}