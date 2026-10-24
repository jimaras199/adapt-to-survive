#include <gtest/gtest.h>

#include "ats/world/Transform2D.hpp"

#include <cmath>
#include <numbers>

TEST(Transform2DTest, DefaultInitialization)
{
    const ats::world::Transform2D transform{};

    EXPECT_DOUBLE_EQ(transform.position.x, 0.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 0.0);
    EXPECT_DOUBLE_EQ(transform.rotationRadians, 0.0);
}

TEST(Transform2DTest, CustomInitialization)
{
    const ats::world::Transform2D transform{
        { 10.0, 20.0 },
        std::numbers::pi / 2.0
    };

    EXPECT_DOUBLE_EQ(transform.position.x, 10.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 20.0);
    EXPECT_DOUBLE_EQ(
        transform.rotationRadians,
        std::numbers::pi / 2.0
    );
}

TEST(Transform2DTest, PositionCanBeUpdated)
{
    ats::world::Transform2D transform{};

    transform.position = { 15.0, -5.0 };

    EXPECT_DOUBLE_EQ(transform.position.x, 15.0);
    EXPECT_DOUBLE_EQ(transform.position.y, -5.0);
}

TEST(Transform2DTest, RotationCanBeUpdated)
{
    ats::world::Transform2D transform{};

    transform.rotationRadians = std::numbers::pi;

    EXPECT_DOUBLE_EQ(
        transform.rotationRadians,
        std::numbers::pi
    );
}


TEST(Transform2DTest, FacingRightAtZeroRadians)
{
    const ats::world::Transform2D transform{};

    const auto direction = transform.FacingDirection();

    EXPECT_NEAR(direction.x, 1.0, 1e-12);
    EXPECT_NEAR(direction.y, 0.0, 1e-12);
}

TEST(Transform2DTest, FacingUpAtHalfPiRadians)
{
    const ats::world::Transform2D transform{
        { 0.0, 0.0 },
        std::numbers::pi / 2.0
    };

    const auto direction = transform.FacingDirection();

    EXPECT_NEAR(direction.x, 0.0, 1e-12);
    EXPECT_NEAR(direction.y, 1.0, 1e-12);
}

TEST(Transform2DTest, FacingLeftAtPiRadians)
{
    const ats::world::Transform2D transform{
        { 0.0, 0.0 },
        std::numbers::pi
    };

    const auto direction = transform.FacingDirection();

    EXPECT_NEAR(direction.x, -1.0, 1e-12);
    EXPECT_NEAR(direction.y, 0.0, 1e-12);
}