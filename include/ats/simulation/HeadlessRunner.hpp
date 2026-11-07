#pragma once

#include <cstdint>

#include "ats/simulation/HeadlessResult.hpp"
#include "ats/simulation/Simulation.hpp"

namespace ats::simulation
{
    class HeadlessRunner
    {
    public:
        explicit HeadlessRunner(Simulation& simulation)
            : simulation_(simulation)
        {
        }

        HeadlessResult Run(std::uint64_t steps)
        {
            for (std::uint64_t i = 0; i < steps; ++i)
            {
                simulation_.Tick();
            }

            return {
                simulation_.CompletedSteps(),
                simulation_.SimulatedTime()
            };
        }

    private:
        Simulation& simulation_;
    };
}

#include <stdexcept>

TEST(HeadlessRunnerTests, FailedTickDoesNotCountAsCompleted)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    std::uint64_t systemCalls = 0;

    simulation.AddSystem(
        [&systemCalls](ats::world::World&, double)
        {
            ++systemCalls;

            if (systemCalls == 3)
            {
                throw std::runtime_error("Simulated system failure");
            }
        }
    );

    ats::simulation::HeadlessRunner runner(simulation);

    EXPECT_THROW(runner.Run(10), std::runtime_error);

    EXPECT_EQ(simulation.CompletedSteps(), 2);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.04, 1e-9);

    const auto result = runner.Run(2);

    EXPECT_EQ(result.completedSteps, 4);
    EXPECT_NEAR(result.simulatedTime, 0.08, 1e-9);
    EXPECT_EQ(systemCalls, 5);
}

TEST(HeadlessRunnerTests, RunsWithEntitiesWithoutMovementComponents)
{
    ats::world::World world;

    const auto entity = world.CreateEntity();

    ats::simulation::Simulation simulation(
        0.02,
        std::move(world)
    );

    ats::simulation::HeadlessRunner runner(simulation);

    const auto result = runner.Run(100);

    EXPECT_EQ(result.completedSteps, 100);
    EXPECT_NEAR(result.simulatedTime, 2.0, 1e-9);

    EXPECT_TRUE(simulation.GetWorld().IsValidEntity(entity));
    EXPECT_EQ(simulation.GetWorld().EntityCount(), 1);
}