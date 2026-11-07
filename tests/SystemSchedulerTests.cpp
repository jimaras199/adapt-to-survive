#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "ats/simulation/SystemScheduler.hpp"
#include "ats/world/World.hpp"

using ats::simulation::SystemScheduler;
using ats::world::World;

TEST(SystemSchedulerTests, EmptySchedulerExecutesSuccessfully)
{
    World world;
    SystemScheduler scheduler;

    EXPECT_NO_THROW(scheduler.Update(world, 0.02));
}

TEST(SystemSchedulerTests, RegisteredSystemExecutesOnce)
{
    World world;
    SystemScheduler scheduler;

    int executionCount = 0;

    scheduler.AddSystem(
        [&executionCount](World&, double)
        {
            ++executionCount;
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_EQ(executionCount, 1);
}

TEST(SystemSchedulerTests, SystemsExecuteInRegistrationOrder)
{
    World world;
    SystemScheduler scheduler;

    std::vector<int> executionOrder;

    scheduler.AddSystem(
        [&executionOrder](World&, double)
        {
            executionOrder.push_back(1);
        }
    );

    scheduler.AddSystem(
        [&executionOrder](World&, double)
        {
            executionOrder.push_back(2);
        }
    );

    scheduler.AddSystem(
        [&executionOrder](World&, double)
        {
            executionOrder.push_back(3);
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_EQ(executionOrder, (std::vector<int>{1, 2, 3}));
}

TEST(SystemSchedulerTests, RejectsEmptySystem)
{
    SystemScheduler scheduler;

    EXPECT_THROW(
        scheduler.AddSystem(SystemScheduler::SystemCallback{}),
        std::invalid_argument
    );
}

TEST(SystemSchedulerTests, SystemsReceiveFixedDeltaTime)
{
    World world;
    SystemScheduler scheduler;

    double receivedDeltaTime = 0.0;

    scheduler.AddSystem(
        [&receivedDeltaTime](World&, double deltaTime)
        {
            receivedDeltaTime = deltaTime;
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_DOUBLE_EQ(receivedDeltaTime, 0.02);
}

TEST(SystemSchedulerTests, SystemCanCreateEntity)
{
    World world;
    SystemScheduler scheduler;

    scheduler.AddSystem(
        [](World& world, double)
        {
            world.CreateEntity();
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_EQ(world.EntityCount(), 1);
}

TEST(SystemSchedulerTests, SystemCanDestroyEntity)
{
    World world;
    SystemScheduler scheduler;

    const entt::entity entity = world.CreateEntity();

    scheduler.AddSystem(
        [entity](World& world, double)
        {
            world.DestroyEntity(entity);
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_FALSE(world.IsValidEntity(entity));
    EXPECT_EQ(world.EntityCount(), 0);
}

TEST(SystemSchedulerTests, WorldChangesAreVisibleToSubsequentSystems)
{
    World world;
    SystemScheduler scheduler;

    std::size_t observedEntityCount = 0;

    scheduler.AddSystem(
        [](World& world, double)
        {
            world.CreateEntity();
        }
    );

    scheduler.AddSystem(
        [&observedEntityCount](World& world, double)
        {
            observedEntityCount = world.EntityCount();
        }
    );

    scheduler.Update(world, 0.02);

    EXPECT_EQ(observedEntityCount, 1);
}

TEST(SystemSchedulerTests, ExceptionStopsSubsequentSystems)
{
    World world;
    SystemScheduler scheduler;

    bool secondSystemExecuted = false;

    scheduler.AddSystem(
        [](World&, double)
        {
            throw std::runtime_error("System failed");
        }
    );

    scheduler.AddSystem(
        [&secondSystemExecuted](World&, double)
        {
            secondSystemExecuted = true;
        }
    );

    EXPECT_THROW(
        scheduler.Update(world, 0.02),
        std::runtime_error
    );

    EXPECT_FALSE(secondSystemExecuted);
}

TEST(SystemSchedulerTests, SchedulerRecoversAfterException)
{
    World world;
    SystemScheduler scheduler;

    bool shouldThrow = true;
    int executionCount = 0;

    scheduler.AddSystem(
        [&shouldThrow, &executionCount](World&, double)
        {
            if (shouldThrow)
            {
                throw std::runtime_error("Temporary failure");
            }

            ++executionCount;
        }
    );

    EXPECT_THROW(
        scheduler.Update(world, 0.02),
        std::runtime_error
    );

    shouldThrow = false;

    EXPECT_NO_THROW(scheduler.Update(world, 0.02));
    EXPECT_EQ(executionCount, 1);
}

TEST(SystemSchedulerTests, RejectsRegistrationDuringUpdate)
{
    World world;
    SystemScheduler scheduler;

    scheduler.AddSystem(
        [&scheduler](World&, double)
        {
            scheduler.AddSystem(
                [](World&, double)
                {
                }
            );
        }
    );

    EXPECT_THROW(
        scheduler.Update(world, 0.02),
        std::logic_error
    );
}

TEST(SystemSchedulerTests, RejectsRecursiveUpdate)
{
    World world;
    SystemScheduler scheduler;

    scheduler.AddSystem(
        [&scheduler](World& world, double deltaTime)
        {
            scheduler.Update(world, deltaTime);
        }
    );

    EXPECT_THROW(
        scheduler.Update(world, 0.02),
        std::logic_error
    );
}