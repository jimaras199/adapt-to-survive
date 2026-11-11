
#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "ats/world/World.hpp"
#include "ats/world/WorldBounds.hpp"

TEST(WorldBoundsTests, DefaultDimensionsAreValid)
{
    EXPECT_NO_THROW(ats::world::World{});
}

TEST(WorldBoundsTests, RejectsZeroWidth)
{
    EXPECT_THROW(
        (ats::world::World{ ats::world::WorldBounds{ 0.0, 500.0 } }),
        std::invalid_argument
    );
}

TEST(WorldBoundsTests, RejectsNegativeHeight)
{
    EXPECT_THROW(
        (ats::world::World{ ats::world::WorldBounds{ 500.0, -1.0 } }),
        std::invalid_argument
    );
}

TEST(WorldBoundsTests, RejectsInfiniteWidth)
{
    const double infinity = std::numeric_limits<double>::infinity();

    EXPECT_THROW(
        (ats::world::World{ ats::world::WorldBounds{ infinity, 500.0 } }),
        std::invalid_argument
    );
}

TEST(WorldBoundsTests, RejectsNaNHeight)
{
    const double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(
        (ats::world::World{ ats::world::WorldBounds{ 500.0, nan } }),
        std::invalid_argument
    );
}
