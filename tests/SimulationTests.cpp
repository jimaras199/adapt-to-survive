
#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "ats/world/EcsMovementSystem.hpp"
#include "ats/simulation/Simulation.hpp"
#include "ats/world/Transform2D.hpp"
#include "ats/world/World.hpp"

using namespace ats::simulation;

TEST(SimulationTests, AcceptsEmptyWorld)
{
    ats::world::World world;

    Simulation simulation(0.02, std::move(world));

    EXPECT_EQ(simulation.GetWorld().EntityCount(), 0);
    EXPECT_EQ(simulation.CompletedSteps(), 0);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), 0.0);
}

TEST(SimulationTests, TakesOwnershipOfPopulatedWorld)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    world.GetRegistry().emplace<ats::world::Transform2D>(entity);

    Simulation simulation(0.02, std::move(world));

    EXPECT_EQ(simulation.GetWorld().EntityCount(), 1);
    EXPECT_TRUE(simulation.GetWorld().IsValidEntity(entity));

    EXPECT_TRUE(
        simulation.GetWorld().GetRegistry()
        .all_of<ats::world::Transform2D>(entity)
    );
}

TEST(SimulationTests, TickAdvancesWithEmptyWorld)
{
    ats::world::World world;
    Simulation simulation(0.02, std::move(world));

    simulation.Tick();
    simulation.Tick();
    simulation.Tick();

    EXPECT_EQ(simulation.CompletedSteps(), 3);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.06, 1e-12);
    EXPECT_EQ(simulation.GetWorld().EntityCount(), 0);
}

TEST(SimulationTests, TickAdvancesWithPopulatedWorld)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    Simulation simulation(0.02, std::move(world));

    simulation.Tick();
    simulation.Tick();

    EXPECT_EQ(simulation.CompletedSteps(), 2);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.04, 1e-12);
    EXPECT_TRUE(simulation.GetWorld().IsValidEntity(entity));
}


TEST(SimulationTests, RejectsInvalidFixedDeltaTime)
{
    ats::world::World world;

    EXPECT_THROW(
        Simulation(0.0, std::move(world)),
        std::invalid_argument
    );
}

TEST(SimulationTests, IdenticalTickCountsProduceIdenticalResults)
{
    ats::world::World firstWorld;
    ats::world::World secondWorld;

    Simulation first(0.02, std::move(firstWorld));
    Simulation second(0.02, std::move(secondWorld));

    for (int i = 0; i < 100; ++i)
    {
        first.Tick();
        second.Tick();
    }

    EXPECT_EQ(first.CompletedSteps(), second.CompletedSteps());
    EXPECT_DOUBLE_EQ(first.SimulatedTime(), second.SimulatedTime());

    EXPECT_EQ(first.CompletedSteps(), 100);
    EXPECT_NEAR(first.SimulatedTime(), 2.0, 1e-12);
}

TEST(SimulationTests, TickingDoesNotModifyEntityComponents)
{
    ats::world::World world;

    const entt::entity entity = world.CreateEntity();

    auto& transform =
        world.GetRegistry().emplace<ats::world::Transform2D>(entity);

    transform.position.x = 12.0;
    transform.position.y = 7.0;
    transform.rotationRadians = 1.5;

    Simulation simulation(0.02, std::move(world));

    for (int i = 0; i < 10; ++i)
    {
        simulation.Tick();
    }

    const auto& storedTransform =
        simulation.GetWorld().GetRegistry()
        .get<ats::world::Transform2D>(entity);

    EXPECT_DOUBLE_EQ(storedTransform.position.x, 12.0);
    EXPECT_DOUBLE_EQ(storedTransform.position.y, 7.0);
    EXPECT_DOUBLE_EQ(storedTransform.rotationRadians, 1.5);

    EXPECT_EQ(simulation.GetWorld().EntityCount(), 1);
    EXPECT_EQ(simulation.CompletedSteps(), 10);
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

TEST(SimulationTests, TickExecutesRegisteredSystems)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    int executionCount = 0;

    simulation.AddSystem(
        [&executionCount](ats::world::World&, double)
        {
            ++executionCount;
        }
    );

    simulation.Tick();
    simulation.Tick();
    simulation.Tick();

    EXPECT_EQ(executionCount, 3);
    EXPECT_EQ(simulation.CompletedSteps(), 3);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), 0.06);
}

TEST(SimulationTests, RegisteredSystemModifiesWorld)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    simulation.AddSystem(
        [](ats::world::World& world, double)
        {
            world.CreateEntity();
        }
    );

    simulation.Tick();

    EXPECT_EQ(simulation.GetWorld().EntityCount(), 1);
}

TEST(SimulationTests, FailedTickDoesNotAdvanceTime)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    simulation.AddSystem(
        [](ats::world::World&, double)
        {
            throw std::runtime_error("System failure");
        }
    );

    EXPECT_THROW(simulation.Tick(), std::runtime_error);

    EXPECT_EQ(simulation.CompletedSteps(), 0);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), 0.0);
}

TEST(SimulationTests, FailedTickDoesNotRollbackWorldChanges)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    simulation.AddSystem(
        [](ats::world::World& world, double)
        {
            world.CreateEntity();
        }
    );

    simulation.AddSystem(
        [](ats::world::World&, double)
        {
            throw std::runtime_error("System failure");
        }
    );

    EXPECT_THROW(simulation.Tick(), std::runtime_error);

    EXPECT_EQ(simulation.GetWorld().EntityCount(), 1);
    EXPECT_EQ(simulation.CompletedSteps(), 0);
}

TEST(SimulationTests, RejectsSystemRegistrationDuringTick)
{
    ats::world::World world;
    ats::simulation::Simulation simulation(0.02, std::move(world));

    simulation.AddSystem(
        [&simulation](ats::world::World&, double)
        {
            simulation.AddSystem(
                [](ats::world::World&, double)
                {
                }
            );
        }
    );

    EXPECT_THROW(simulation.Tick(), std::logic_error);

    EXPECT_EQ(simulation.CompletedSteps(), 0);
    EXPECT_DOUBLE_EQ(simulation.SimulatedTime(), 0.0);
}

TEST(SimulationTests, TickUpdatesEcsMovement)
{
    ats::world::World world;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::Transform2D>(entity);

    registry.emplace<ats::world::MovementInput>(entity, ats::math::Vector2{ 1.0, 0.0 }, 0.0);

    registry.emplace<ats::world::MovementConfig>(entity, 3.0, 2.0);

    ats::simulation::Simulation simulation(0.02, std::move(world));

    simulation.AddSystem(
        [](ats::world::World& world, double deltaTime)
        {
            ats::world::EcsMovementSystem movementSystem;
            movementSystem.Update(world, deltaTime);
        }
    );

    simulation.Tick();
    simulation.Tick();
    simulation.Tick();

    const auto& transform = simulation.GetWorld().GetRegistry().get<ats::world::Transform2D>(entity);

    EXPECT_NEAR(transform.position.x, 0.18, 1e-9);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-9);

    EXPECT_EQ(simulation.CompletedSteps(), 3);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.06, 1e-9);
}

TEST(SimulationTests, MovementIsNotUpdatedWithoutRegistration)
{
    ats::world::World world;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<ats::world::Transform2D>(entity);

    registry.emplace<ats::world::MovementInput>(
        entity,
        ats::math::Vector2{ 1.0, 0.0 },
        0.0
    );

    registry.emplace<ats::world::MovementConfig>(
        entity,
        3.0,
        2.0
    );

    ats::simulation::Simulation simulation(
        0.02,
        std::move(world)
    );

    simulation.Tick();
    simulation.Tick();
    simulation.Tick();

    const auto& transform =
        simulation.GetWorld()
        .GetRegistry()
        .get<ats::world::Transform2D>(entity);

    EXPECT_DOUBLE_EQ(transform.position.x, 0.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 0.0);

    EXPECT_EQ(simulation.CompletedSteps(), 3);
    EXPECT_NEAR(simulation.SimulatedTime(), 0.06, 1e-9);
}