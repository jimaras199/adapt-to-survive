#pragma once

namespace ats::math
{
    struct Vector2
    {
        double x = 0.0;
        double y = 0.0;

        constexpr Vector2() = default;

        constexpr Vector2(double xValue, double yValue)
            : x(xValue), y(yValue)
        {
        }

        [[nodiscard]] constexpr Vector2 operator+(const Vector2& other) const
        {
            return { x + other.x, y + other.y };
        }

        [[nodiscard]] constexpr Vector2 operator-(const Vector2& other) const
        {
            return { x - other.x, y - other.y };
        }

        [[nodiscard]] constexpr Vector2 operator*(double scalar) const
        {
            return { x * scalar, y * scalar };
        }
    };
}
