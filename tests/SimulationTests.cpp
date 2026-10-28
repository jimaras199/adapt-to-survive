#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "ats/simulation/MovementCommand.hpp"
#include "ats/simulation/Simulation.hpp"
#include "ats/world/World.hpp"

using namespace ats::simulation;

TEST(SimulationTests, StartsWithDefaultWorldAndZeroCompletedSteps)
{
    Simulation simulation(0.02, 5.0, 2.0);

    const auto& agent = simulation.GetWorld().GetAgent();

    EXPECT_DOUBLE_EQ(agent.transform.position.x, 0.0);
    EXPECT_DOUBLE_EQ(agent.transform.position.y, 0.0);
    EXPECT_DOUBLE_EQ(agent.transform.rotationRadians, 0.0);

    EXPECT_EQ(simulation.CompletedSteps(), 0);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), 0.0);
}

TEST(SimulationTests, MovesAgentForwardForOneStep)
{
    Simulation simulation(0.02, 5.0, 2.0);

    MovementCommand command{
        .localMovement = { 1.0, 0.0 },
        .rotationInput = 0.0
    };

    simulation.Step(command);

    const auto& position =
        simulation.GetWorld().GetAgent().transform.position;

    EXPECT_NEAR(position.x, 0.1, 1e-12);
    EXPECT_NEAR(position.y, 0.0, 1e-12);
}

TEST(SimulationTests, RotatesAgentForOneStep)
{
    Simulation simulation(0.02, 5.0, 2.0);

    MovementCommand command{
        .localMovement = { 0.0, 0.0 },
        .rotationInput = 1.0
    };

    simulation.Step(command);

    const double rotation =
        simulation.GetWorld().GetAgent().transform.rotationRadians;

    EXPECT_NEAR(rotation, 0.04, 1e-12);
}

TEST(SimulationTests, AdvancesStepCounterAndSimulatedTime)
{
    Simulation simulation(0.02, 5.0, 2.0);

    MovementCommand command{};

    simulation.Step(command);
    simulation.Step(command);
    simulation.Step(command);

    EXPECT_EQ(simulation.CompletedSteps(), 3);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.06, 1e-12);
}


TEST(SimulationTests, RejectsInvalidMovementSpeed)
{
    EXPECT_THROW(
        {
            Simulation simulation(0.02, -1.0, 2.0);
        },
        std::invalid_argument
    );

    EXPECT_THROW(
        {
            Simulation simulation(
                0.02,
                std::numeric_limits<double>::infinity(),
                2.0
            );
        },
        std::invalid_argument
    );
}

TEST(SimulationTests, RejectsInvalidRotationSpeed)
{
    EXPECT_THROW(
        {
            Simulation simulation(0.02, 5.0, -1.0);
        },
        std::invalid_argument
    );

    EXPECT_THROW(
        {
            Simulation simulation(
                0.02,
                5.0,
                std::numeric_limits<double>::quiet_NaN()
            );
        },
        std::invalid_argument
    );
}

TEST(SimulationTests, RejectsInvalidFixedDeltaTime)
{
    EXPECT_THROW(
        {
            Simulation simulation(0.0, 5.0, 2.0);
        },
        std::invalid_argument
    );
}

TEST(SimulationTests, InvalidCommandDoesNotChangeState)
{
    Simulation simulation(0.02, 5.0, 2.0);

    MovementCommand validCommand{
        .localMovement = { 1.0, 0.0 },
        .rotationInput = 0.0
    };

    simulation.Step(validCommand);

    const auto before =
        simulation.GetWorld().GetAgent().transform;

    const auto stepsBefore = simulation.CompletedSteps();
    const auto timeBefore = simulation.SimulatedTime();

    MovementCommand invalidCommand{
        .localMovement = {
            std::numeric_limits<double>::quiet_NaN(),
            0.0
        },
        .rotationInput = 0.0
    };

    EXPECT_THROW(
        {
            simulation.Step(invalidCommand);
        },
        std::invalid_argument
    );

    const auto& after =
        simulation.GetWorld().GetAgent().transform;

    EXPECT_DOUBLE_EQ(after.position.x, before.position.x);
    EXPECT_DOUBLE_EQ(after.position.y, before.position.y);
    EXPECT_DOUBLE_EQ(
        after.rotationRadians,
        before.rotationRadians
    );

    EXPECT_EQ(simulation.CompletedSteps(), stepsBefore);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), timeBefore);
}

TEST(SimulationTests, IdenticalCommandsProduceIdenticalResults)
{
    Simulation first(0.02, 5.0, 2.0);
    Simulation second(0.02, 5.0, 2.0);

    const MovementCommand commands[]{
        { { 1.0, 0.0 }, 0.0 },
        { { 1.0, 1.0 }, 0.5 },
        { { 0.0, -1.0 }, -1.0 },
        { { -1.0, 0.0 }, 0.0 }
    };

    for (const auto& command : commands)
    {
        first.Step(command);
        second.Step(command);
    }

    const auto& firstTransform =
        first.GetWorld().GetAgent().transform;

    const auto& secondTransform =
        second.GetWorld().GetAgent().transform;

    EXPECT_DOUBLE_EQ(
        firstTransform.position.x,
        secondTransform.position.x
    );

    EXPECT_DOUBLE_EQ(
        firstTransform.position.y,
        secondTransform.position.y
    );

    EXPECT_DOUBLE_EQ(
        firstTransform.rotationRadians,
        secondTransform.rotationRadians
    );

    EXPECT_EQ(first.CompletedSteps(), second.CompletedSteps());
    EXPECT_DOUBLE_EQ(first.SimulatedTime(), second.SimulatedTime());
}

TEST(SimulationTests, ExposesWorldAsConstReference)
{
    using ReturnType = decltype(
        std::declval<Simulation&>().GetWorld()
        );

    static_assert(
        std::is_same_v<ReturnType, const ats::world::World&>
        );
}