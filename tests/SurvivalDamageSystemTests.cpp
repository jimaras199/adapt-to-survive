#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/SurvivalDamageConfig.hpp"
#include "ats/world/SurvivalDamageSystem.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

TEST(SurvivalDamageSystemTests, StarvationProducesInternalDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 10.0);

    EXPECT_NEAR(accumulator.internalDamage, 100.0 / 6.0, 1e-9);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);

    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalDamageSystemTests, DehydrationProducesEqualDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.water = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 10.0);

    EXPECT_NEAR(accumulator.internalDamage, 100.0 / 6.0, 1e-9);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(SurvivalDamageSystemTests, StarvationAndDehydrationStack)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 0.0;
    state.water = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 10.0);

    EXPECT_NEAR(accumulator.internalDamage, 100.0 / 3.0, 1e-9);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(SurvivalDamageSystemTests, HealthyNeedsProduceNoDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    registry.emplace<ats::world::SurvivalState>(entity);

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 10.0);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(SurvivalDamageSystemTests, PreservesExistingDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 5.0;
    accumulator.externalDamage = 12.0;

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 6.0);

    EXPECT_NEAR(accumulator.internalDamage, 15.0, 1e-9);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 12.0);
}

TEST(SurvivalDamageSystemTests, RejectsInvalidDamageRates)
{
    EXPECT_THROW(
        ats::world::SurvivalDamageSystem(
            ats::world::SurvivalDamageConfig{ -1.0 }
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        ats::world::SurvivalDamageSystem(
            ats::world::SurvivalDamageConfig{
                std::numeric_limits<double>::infinity()
            }
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        ats::world::SurvivalDamageSystem(
            ats::world::SurvivalDamageConfig{
                std::numeric_limits<double>::quiet_NaN()
            }
        ),
        std::invalid_argument
    );
}

TEST(SurvivalDamageSystemTests, MultipleEntitiesReceiveIndependentDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    auto& stateA = registry.emplace<ats::world::SurvivalState>(entityA);
    auto& stateB = registry.emplace<ats::world::SurvivalState>(entityB);

    auto& damageA = registry.emplace<ats::world::DamageAccumulator>(entityA);
    auto& damageB = registry.emplace<ats::world::DamageAccumulator>(entityB);

    stateA.food = 0.0;

    stateB.food = 0.0;
    stateB.water = 0.0;

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 6.0);

    EXPECT_NEAR(damageA.internalDamage, 10.0, 1e-9);
    EXPECT_NEAR(damageB.internalDamage, 20.0, 1e-9);

    EXPECT_DOUBLE_EQ(damageA.externalDamage, 0.0);
    EXPECT_DOUBLE_EQ(damageB.externalDamage, 0.0);
}

TEST(SurvivalDamageSystemTests, ZeroDeltaTimeProducesNoAdditionalDamage)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 0.0;
    state.water = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    accumulator.internalDamage = 5.0;

    ats::world::SurvivalDamageSystem system;

    system.Update(world, 0.0);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 5.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
}

TEST(SurvivalDamageSystemTests, IgnoresEntitiesMissingRequiredComponents)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto survivalOnly = world.CreateEntity();
    const auto accumulatorOnly = world.CreateEntity();

    auto& state =
        registry.emplace<ats::world::SurvivalState>(survivalOnly);

    state.food = 0.0;
    state.water = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(accumulatorOnly);

    ats::world::SurvivalDamageSystem system;

    EXPECT_NO_THROW(system.Update(world, 1.0));

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 0.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalDamageSystemTests, RejectsInvalidDeltaTime)
{
    ats::world::World world;
    ats::world::SurvivalDamageSystem system;

    EXPECT_THROW(
        system.Update(world, -0.02),
        std::invalid_argument
    );

    EXPECT_THROW(
        system.Update(
            world,
            std::numeric_limits<double>::infinity()
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        system.Update(
            world,
            std::numeric_limits<double>::quiet_NaN()
        ),
        std::invalid_argument
    );
}

TEST(SurvivalDamageSystemTests, SupportsCustomDamageRate)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 0.0;
    state.water = 0.0;

    auto& accumulator =
        registry.emplace<ats::world::DamageAccumulator>(entity);

    ats::world::SurvivalDamageSystem system(
        ats::world::SurvivalDamageConfig{ 2.5 }
    );

    system.Update(world, 4.0);

    EXPECT_DOUBLE_EQ(accumulator.internalDamage, 20.0);
    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}