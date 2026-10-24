
#include <gtest/gtest.h>

#include "ats/simulation/FixedStepRunner.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

using ats::simulation::FixedStepRunner;

TEST(FixedStepRunnerTest, StoresFixedDeltaTime)
{
    FixedStepRunner runner(1.0 / 60.0);

    EXPECT_DOUBLE_EQ(runner.FixedDeltaTime(), 1.0 / 60.0);
    EXPECT_EQ(runner.CompletedSteps(), 0u);
    EXPECT_DOUBLE_EQ(runner.SimulatedTime(), 0.0);
}

TEST(FixedStepRunnerTest, ExecutesRequestedNumberOfSteps)
{
    FixedStepRunner runner(0.02);
    int calls = 0;

    runner.RunSteps(3, [&](double)
        {
            ++calls;
        });

    EXPECT_EQ(calls, 3);
    EXPECT_EQ(runner.CompletedSteps(), 3u);
}

TEST(FixedStepRunnerTest, ZeroStepsDoesNotInvokeCallback)
{
    FixedStepRunner runner(0.02);
    int calls = 0;

    runner.RunSteps(0, [&](double)
        {
            ++calls;
        });

    EXPECT_EQ(calls, 0);
    EXPECT_EQ(runner.CompletedSteps(), 0u);
}

TEST(FixedStepRunnerTest, SuppliesConstantDeltaTime)
{
    FixedStepRunner runner(0.02);
    std::vector<double> receivedDeltaTimes;

    runner.RunSteps(5, [&](double deltaTime)
        {
            receivedDeltaTimes.push_back(deltaTime);
        });

    ASSERT_EQ(receivedDeltaTimes.size(), 5u);

    for (double deltaTime : receivedDeltaTimes)
    {
        EXPECT_DOUBLE_EQ(deltaTime, 0.02);
    }
}

TEST(FixedStepRunnerTest, AccumulatesCompletedStepsAcrossCalls)
{
    FixedStepRunner runner(0.02);

    runner.RunSteps(3, [](double) {});
    runner.RunSteps(7, [](double) {});

    EXPECT_EQ(runner.CompletedSteps(), 10u);
}

TEST(FixedStepRunnerTest, CalculatesSimulatedTime)
{
    FixedStepRunner runner(0.25);

    runner.RunSteps(8, [](double) {});

    EXPECT_DOUBLE_EQ(runner.SimulatedTime(), 2.0);
}

TEST(FixedStepRunnerTest, RejectsZeroDeltaTime)
{
    EXPECT_THROW(
        {
            FixedStepRunner runner(0.0);
        },
        std::invalid_argument
    );
}

TEST(FixedStepRunnerTest, RejectsNegativeDeltaTime)
{
    EXPECT_THROW(
        {
            FixedStepRunner runner(-0.02);
        },
        std::invalid_argument
    );
}

TEST(FixedStepRunnerTest, RejectsNaNDeltaTime)
{
    const double nan =
        std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(
        {
            FixedStepRunner runner(nan);
        },
        std::invalid_argument
    );
}

TEST(FixedStepRunnerTest, RejectsInfiniteDeltaTime)
{
    const double infinity =
        std::numeric_limits<double>::infinity();

    EXPECT_THROW(
        {
            FixedStepRunner runner(infinity);
        },
        std::invalid_argument
    );
}

TEST(FixedStepRunnerTest, DoesNotCountFailedStep)
{
    FixedStepRunner runner(0.02);
    int calls = 0;

    EXPECT_THROW(
        {
            runner.RunSteps(5, [&](double)
            {
                ++calls;

                if (calls == 3)
                {
                    throw std::runtime_error("Simulated failure");
                }
            });
        },
        std::runtime_error
    );

    EXPECT_EQ(calls, 3);
    EXPECT_EQ(runner.CompletedSteps(), 2u);
}

TEST(FixedStepRunnerTest, RejectsCounterOverflow)
{
    FixedStepRunner runner(0.02);

    runner.RunSteps(1, [](double) {});

    bool callbackCalled = false;

    EXPECT_THROW(
        {
            runner.RunSteps(
                std::numeric_limits<std::uint64_t>::max(),
                [&](double)
                {
                    callbackCalled = true;
                }
            );
        },
        std::overflow_error
    );

    EXPECT_FALSE(callbackCalled);
    EXPECT_EQ(runner.CompletedSteps(), 1u);
}