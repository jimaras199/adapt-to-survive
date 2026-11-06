
#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>
#include <utility>

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
