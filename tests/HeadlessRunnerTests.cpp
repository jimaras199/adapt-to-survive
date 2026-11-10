#include <cstdint>
#include <utility>

#include <gtest/gtest.h>

#include "ats/simulation/HeadlessRunner.hpp"
#include "ats/simulation/Simulation.hpp"
#include "ats/world/World.hpp"
#include "ats/world/SurvivalDepletionSystem.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/DamageResetSystem.hpp"
#include "ats/world/HealthApplicationSystem.hpp"
#include "ats/world/SurvivalDamageSystem.hpp"

TEST(HeadlessRunnerTests, ZeroStepsDoesNotAdvanceSimulation)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));
    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(0);

    EXPECT_EQ(result.completedSteps, 0);
    EXPECT_DOUBLE_EQ(result.simulatedTime, 0.0);

    EXPECT_EQ(simulation.CompletedSteps(), 0);
}

TEST(HeadlessRunnerTests, RunsRequestedNumberOfSteps)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));
    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(50);

    EXPECT_EQ(result.completedSteps, 50);
    EXPECT_NEAR(result.simulatedTime, 1.0, 1e-9);
}

TEST(HeadlessRunnerTests, MultipleRunsAccumulateSteps)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));
    ats::simulation::HeadlessRunner runner(simulation);

    const auto firstResult = runner.Run(100);
    const auto secondResult = runner.Run(50);

    EXPECT_EQ(firstResult.completedSteps, 100);
    EXPECT_NEAR(firstResult.simulatedTime, 2.0, 1e-9);

    EXPECT_EQ(secondResult.completedSteps, 150);
    EXPECT_NEAR(secondResult.simulatedTime, 3.0, 1e-9);

    EXPECT_EQ(simulation.CompletedSteps(), 150);
}

TEST(HeadlessRunnerTests, ExecutesRegisteredSystems)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    std::uint64_t systemCalls = 0;

    simulation.AddSystem(
        [&systemCalls](ats::world::World&, double)
        {
            ++systemCalls;
        }
    );

    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(25);

    EXPECT_EQ(systemCalls, 25);
    EXPECT_EQ(result.completedSteps, 25);
    EXPECT_NEAR(result.simulatedTime, 0.5, 1e-9);
}

TEST(HeadlessRunnerTests, DepletesSurvivalReservesDuringHeadlessExecution)
{
    ats::world::World world;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::SurvivalState>(entity);

    ats::simulation::Simulation simulation(
        0.02,
        std::move(world)
    );

    ats::world::SurvivalDepletionSystem depletionSystem;

    simulation.AddSystem(
        [depletionSystem](ats::world::World& world, double deltaTime)
        {
            depletionSystem.Update(world, deltaTime);
        }
    );

    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(3000);

    const auto& state =
        simulation.GetWorld()
        .GetRegistry()
        .get<ats::world::SurvivalState>(entity);

    EXPECT_NEAR(state.food, 0.0, 1e-9);
    EXPECT_DOUBLE_EQ(state.water, 0.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);

    EXPECT_EQ(result.completedSteps, 3000);
    EXPECT_NEAR(result.simulatedTime, 60.0, 1e-9);
}

TEST(HeadlessRunnerTests, StackedSurvivalDamageReachesZeroHealth)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    auto& state =
        registry.emplace<ats::world::SurvivalState>(entity);

    registry.emplace<ats::world::DamageAccumulator>(entity);

    state.food = 0.0;
    state.water = 0.0;

    ats::simulation::Simulation simulation(
        0.02,
        std::move(world)
    );

    ats::world::DamageResetSystem resetSystem;
    ats::world::SurvivalDepletionSystem depletionSystem;
    ats::world::SurvivalDamageSystem damageSystem;
    ats::world::HealthApplicationSystem healthSystem;

    simulation.AddSystem(
        [resetSystem](ats::world::World& world, double)
        {
            resetSystem.Update(world);
        }
    );

    simulation.AddSystem(
        [depletionSystem](ats::world::World& world, double deltaTime)
        {
            depletionSystem.Update(world, deltaTime);
        }
    );

    simulation.AddSystem(
        [damageSystem](ats::world::World& world, double deltaTime)
        {
            damageSystem.Update(world, deltaTime);
        }
    );

    simulation.AddSystem(
        [healthSystem](ats::world::World& world, double)
        {
            healthSystem.Update(world);
        }
    );

    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(1500);

    const auto& finalState =
        simulation.GetWorld()
        .GetRegistry()
        .get<ats::world::SurvivalState>(entity);

    const auto& accumulator =
        simulation.GetWorld()
        .GetRegistry()
        .get<ats::world::DamageAccumulator>(entity);

    EXPECT_NEAR(finalState.health, 0.0, 1e-9);

    EXPECT_DOUBLE_EQ(finalState.food, 0.0);
    EXPECT_DOUBLE_EQ(finalState.water, 0.0);

    EXPECT_NEAR(
        accumulator.internalDamage,
        2.0 * (100.0 / 60.0) * 0.02,
        1e-12
    );

    EXPECT_DOUBLE_EQ(accumulator.externalDamage, 0.0);

    EXPECT_EQ(result.completedSteps, 1500);
    EXPECT_NEAR(result.simulatedTime, 30.0, 1e-9);
}