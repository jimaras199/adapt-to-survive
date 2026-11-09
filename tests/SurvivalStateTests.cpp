#include <gtest/gtest.h>

#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

TEST(SurvivalStateTests, DefaultValuesAreFull)
{
    ats::world::SurvivalState state{};

    EXPECT_DOUBLE_EQ(state.food, 100.0);
    EXPECT_DOUBLE_EQ(state.water, 100.0);
    EXPECT_DOUBLE_EQ(state.health, 100.0);
}

TEST(SurvivalStateTests, SupportsCustomInitialValues)
{
    ats::world::SurvivalState state{
        75.0,
        50.0,
        25.0
    };

    EXPECT_DOUBLE_EQ(state.food, 75.0);
    EXPECT_DOUBLE_EQ(state.water, 50.0);
    EXPECT_DOUBLE_EQ(state.health, 25.0);
}

TEST(SurvivalStateTests, EntitiesHaveIndependentSurvivalStates)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    auto& stateA = registry.emplace<ats::world::SurvivalState>(entityA);
    auto& stateB = registry.emplace<ats::world::SurvivalState>(entityB);

    stateA.food = 30.0;
    stateA.water = 60.0;

    EXPECT_DOUBLE_EQ(stateA.food, 30.0);
    EXPECT_DOUBLE_EQ(stateA.water, 60.0);

    EXPECT_DOUBLE_EQ(stateB.food, 100.0);
    EXPECT_DOUBLE_EQ(stateB.water, 100.0);
}

TEST(SurvivalStateTests, EntityCanExistWithoutSurvivalState)
{
    ats::world::World world;
    auto& registry = world.GetRegistry();

    const auto entity = world.CreateEntity();

    EXPECT_FALSE(
        registry.all_of<ats::world::SurvivalState>(entity)
    );
}