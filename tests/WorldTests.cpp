#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

#include "ats/world/World.hpp"

using namespace ats::world;

TEST(WorldTests, DefaultAgentStartsAtOrigin)
{
    World world;

    const Agent& agent = world.GetAgent();

    EXPECT_DOUBLE_EQ(agent.transform.position.x, 0.0);
    EXPECT_DOUBLE_EQ(agent.transform.position.y, 0.0);
    EXPECT_DOUBLE_EQ(agent.transform.rotationRadians, 0.0);
}

TEST(WorldTests, GetAgentReturnsReferenceToSameObject)
{
    World world;

    const Agent& first = world.GetAgent();
    const Agent& second = world.GetAgent();

    EXPECT_EQ(&first, &second);
}

TEST(WorldTests, GetAgentReturnsConstReference)
{
    using ReturnType = decltype(std::declval<World&>().GetAgent());

    static_assert(std::is_same_v<ReturnType, const Agent&>);
}

TEST(WorldTests, AgentTransformCannotBeModifiedThroughGetter)
{
    using ReturnType = decltype(
        (std::declval<World&>().GetAgent().transform)
        );

    static_assert(std::is_const_v<
        std::remove_reference_t<ReturnType>
    >);
}