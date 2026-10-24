
#include <gtest/gtest.h>

#include "ats/world/MovementSystem.hpp"

#include <cmath>
#include <numbers>
#include <limits>
#include <stdexcept>

using ats::math::Vector2;
using ats::world::MovementSystem;
using ats::world::Transform2D;

TEST(MovementSystemTest, MovesForward)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform,
        { 1.0, 0.0 }, // Forward
        0.0,        // No rotation input
        4.0,        // Movement speed
        0.0,        // Rotation speed
        0.5         // Delta time
    );

    EXPECT_NEAR(transform.position.x, 2.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);
}

TEST(MovementSystemTest, MovesBackward)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { -1.0, 0.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(transform.position.x, -2.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);
}

TEST(MovementSystemTest, StrafesLeft)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 0.0, 1.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(transform.position.x, 0.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 2.0, 1e-12);
}

TEST(MovementSystemTest, ForwardMovementUsesOrientation)
{
    Transform2D transform{
        { 10.0, 20.0 },
        std::numbers::pi / 2.0
    };

    MovementSystem::Update(
        transform, { 1.0, 0.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(transform.position.x, 10.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 22.0, 1e-12);
}

TEST(MovementSystemTest, NormalizesDiagonalMovement)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 1.0, 1.0 },
        0.0, 4.0, 0.0, 0.5
    );

    const double distance = std::sqrt(
        transform.position.x * transform.position.x +
        transform.position.y * transform.position.y
    );

    EXPECT_NEAR(distance, 2.0, 1e-12);
}

TEST(MovementSystemTest, PreservesPartialMovementInput)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 0.5, 0.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(transform.position.x, 1.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);
}

TEST(MovementSystemTest, RotatesWithoutMoving)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 0.0, 0.0 },
        1.0, 0.0, std::numbers::pi, 0.5
    );

    EXPECT_NEAR(transform.position.x, 0.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);
    EXPECT_NEAR(transform.rotationRadians,
        std::numbers::pi / 2.0, 1e-12);
}

TEST(MovementSystemTest, MovesUsingStartingOrientation)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 1.0, 0.0 },
        1.0, 4.0, std::numbers::pi, 0.5
    );

    // Translation uses the initial orientation (0 radians).
    EXPECT_NEAR(transform.position.x, 2.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);

    // Rotation is applied after translation.
    EXPECT_NEAR(transform.rotationRadians,
        std::numbers::pi / 2.0, 1e-12);
}

TEST(MovementSystemTest, ZeroInputDoesNotChangeTransform)
{
    Transform2D transform{
        { 10.0, 20.0 },
        std::numbers::pi / 4.0
    };

    MovementSystem::Update(
        transform, { 0.0, 0.0 },
        0.0, 4.0, 2.0, 0.5
    );

    EXPECT_DOUBLE_EQ(transform.position.x, 10.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 20.0);
    EXPECT_DOUBLE_EQ(transform.rotationRadians,
        std::numbers::pi / 4.0);
}

TEST(MovementSystemTest, ZeroDeltaTimeDoesNotChangeTransform)
{
    Transform2D transform{
        { 10.0, 20.0 },
        0.0
    };

    MovementSystem::Update(
        transform, { 1.0, 0.0 },
        1.0, 4.0, 2.0, 0.0
    );

    EXPECT_DOUBLE_EQ(transform.position.x, 10.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 20.0);
    EXPECT_DOUBLE_EQ(transform.rotationRadians, 0.0);
}

TEST(MovementSystemTest, DisplacementScalesWithDeltaTime)
{
    Transform2D shortStep{};
    Transform2D longStep{};

    MovementSystem::Update(
        shortStep, { 1.0, 0.0 },
        0.0, 4.0, 0.0, 0.25
    );

    MovementSystem::Update(
        longStep, { 1.0, 0.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(shortStep.position.x, 1.0, 1e-12);
    EXPECT_NEAR(longStep.position.x, 2.0, 1e-12);
}

TEST(MovementSystemTest, ClampsMovementInput)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 2.0, 0.0 },
        0.0, 4.0, 0.0, 0.5
    );

    EXPECT_NEAR(transform.position.x, 2.0, 1e-12);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-12);
}

TEST(MovementSystemTest, ClampsRotationInput)
{
    Transform2D transform{};

    MovementSystem::Update(
        transform, { 0.0, 0.0 },
        3.0, 0.0, 2.0, 0.5
    );

    EXPECT_NEAR(transform.rotationRadians, 1.0, 1e-12);
}

TEST(MovementSystemTest, RejectsNegativeSpeed)
{
    Transform2D transform{};

    EXPECT_THROW(
        MovementSystem::Update(
            transform, { 1.0, 0.0 },
            0.0, -4.0, 0.0, 0.5
        ),
        std::invalid_argument
    );
}

TEST(MovementSystemTest, RejectsNegativeDeltaTime)
{
    Transform2D transform{};

    EXPECT_THROW(
        MovementSystem::Update(
            transform, { 1.0, 0.0 },
            0.0, 4.0, 0.0, -0.5
        ),
        std::invalid_argument
    );
}

TEST(MovementSystemTest, RejectsNonFiniteInput)
{
    Transform2D transform{};

    const double nan =
        std::numeric_limits<double>::quiet_NaN();

    EXPECT_THROW(
        MovementSystem::Update(
            transform, { nan, 0.0 },
            0.0, 4.0, 0.0, 0.5
        ),
        std::invalid_argument
    );
}