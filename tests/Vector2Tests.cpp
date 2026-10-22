
#include <gtest/gtest.h>
#include "ats/math/Vector2.hpp"

TEST(Vector2Test, DefaultConstructor)
{
    const ats::math::Vector2 vector;

    EXPECT_DOUBLE_EQ(vector.x, 0.0);
    EXPECT_DOUBLE_EQ(vector.y, 0.0);
}

TEST(Vector2Test, ParameterizedConstructor)
{
    const ats::math::Vector2 vector{ 10.0, -5.0 };

    EXPECT_DOUBLE_EQ(vector.x, 10.0);
    EXPECT_DOUBLE_EQ(vector.y, -5.0);
}

TEST(Vector2Test, Addition)
{
    const ats::math::Vector2 a{ 10.0, 20.0 };
    const ats::math::Vector2 b{ 3.0, -2.0 };

    const auto result = a + b;

    EXPECT_DOUBLE_EQ(result.x, 13.0);
    EXPECT_DOUBLE_EQ(result.y, 18.0);
}

TEST(Vector2Test, Subtraction)
{
    const ats::math::Vector2 a{ 10.0, 20.0 };
    const ats::math::Vector2 b{ 3.0, 5.0 };

    const auto result = a - b;

    EXPECT_DOUBLE_EQ(result.x, 7.0);
    EXPECT_DOUBLE_EQ(result.y, 15.0);
}

TEST(Vector2Test, ScalarMultiplication)
{
    const ats::math::Vector2 vector{ 3.0, -4.0 };

    const auto result = vector * 2.0;

    EXPECT_DOUBLE_EQ(result.x, 6.0);
    EXPECT_DOUBLE_EQ(result.y, -8.0);
}

TEST(Vector2Test, MultiplicationByZero)
{
    const ats::math::Vector2 vector{ 3.0, -4.0 };

    const auto result = vector * 0.0;

    EXPECT_DOUBLE_EQ(result.x, 0.0);
    EXPECT_DOUBLE_EQ(result.y, 0.0);
}

TEST(Vector2Test, MultiplicationByNegativeScalar)
{
    const ats::math::Vector2 vector{ 3.0, -4.0 };

    const auto result = vector * -2.0;

    EXPECT_DOUBLE_EQ(result.x, -6.0);
    EXPECT_DOUBLE_EQ(result.y, 8.0);
}
