#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "ats/world/SurvivalDepletionConfig.hpp"
#include "ats/world/SurvivalDepletionSystem.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

TEST(SurvivalDepletionSystemTests, DefaultRatesMatchConfiguration)
{
    ats::world::SurvivalDepletionConfig config{};

    EXPECT_NEAR(config.foodRate, 100.0 / 60.0, 1e-12);
    EXPECT_NEAR(config.waterRate, (100.0 / 60.0) * 1.15, 1e-12);
}

TEST(SurvivalDepletionSystemTests, FoodAndWaterDepleteIndependently)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();
    auto& state = registry.emplace<ats::world::SurvivalState>(entity);

    ats::world::SurvivalDepletionSystem system;

    system.Update(world, 10.0);

    EXPECT_NEAR(state.food, 83.3333333333, 1e-9);
    EXPECT_NEAR(state.water, 80.8333333333, 1e-9);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalDepletionSystemTests, ReservesAreClampedToZero)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state = registry.emplace<ats::world::SurvivalState>(entity);
    state.food = 5.0;
    state.water = 3.0;

    ats::world::SurvivalDepletionSystem system;

    system.Update(world, 10.0);

    EXPECT_DOUBLE_EQ(state.food, 0.0);
    EXPECT_DOUBLE_EQ(state.water, 0.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalDepletionSystemTests, RejectsNegativeDepletionRates)
{
    EXPECT_THROW(
        ats::world::SurvivalDepletionSystem(
            ats::world::SurvivalDepletionConfig{ -1.0, 1.0 }
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        ats::world::SurvivalDepletionSystem(
            ats::world::SurvivalDepletionConfig{ 1.0, -1.0 }
        ),
        std::invalid_argument
    );
}

TEST(SurvivalDepletionSystemTests, RejectsInvalidDeltaTime)
{
    ats::world::World world;
    ats::world::SurvivalDepletionSystem system;

    EXPECT_THROW(system.Update(world, -0.02), std::invalid_argument);

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

TEST(SurvivalDepletionSystemTests, MultipleEntitiesDepleteIndependently)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    auto& stateA = registry.emplace<ats::world::SurvivalState>(entityA);
    auto& stateB = registry.emplace<ats::world::SurvivalState>(entityB);

    stateA.food = 100.0;
    stateA.water = 100.0;

    stateB.food = 50.0;
    stateB.water = 25.0;

    ats::world::SurvivalDepletionSystem system;

    system.Update(world, 10.0);

    EXPECT_NEAR(stateA.food, 83.3333333333, 1e-9);
    EXPECT_NEAR(stateA.water, 80.8333333333, 1e-9);

    EXPECT_NEAR(stateB.food, 33.3333333333, 1e-9);
    EXPECT_NEAR(stateB.water, 5.8333333333, 1e-9);
}

TEST(SurvivalDepletionSystemTests, SupportsCustomDepletionRates)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();
    auto& state = registry.emplace<ats::world::SurvivalState>(entity);

    ats::world::SurvivalDepletionConfig config{ 2.0,4.0 };

    ats::world::SurvivalDepletionSystem system(config);

    system.Update(world, 5.0);

    EXPECT_DOUBLE_EQ(state.food, 90.0);
    EXPECT_DOUBLE_EQ(state.water, 80.0);
}

TEST(SurvivalDepletionSystemTests, ZeroDeltaTimeDoesNotDepleteReserves)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();
    auto& state = registry.emplace<ats::world::SurvivalState>(entity);

    state.food = 75.0;
    state.water = 50.0;

    ats::world::SurvivalDepletionSystem system;

    system.Update(world, 0.0);

    EXPECT_DOUBLE_EQ(state.food, 75.0);
    EXPECT_DOUBLE_EQ(state.water, 50.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalDepletionSystemTests, FoodDepletesAfterSixtySeconds)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();
    auto& state = registry.emplace<ats::world::SurvivalState>(entity);

    ats::world::SurvivalDepletionSystem system;

    constexpr double deltaTime = 0.02;
    constexpr int totalSteps = 3000;

    for (int i = 0; i < totalSteps; ++i)
    {
        system.Update(world, deltaTime);
    }

    EXPECT_NEAR(state.food, 0.0, 1e-9);
    EXPECT_DOUBLE_EQ(state.water, 0.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}