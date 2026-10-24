#pragma once

#include <cmath>
#include "ats/math/Vector2.hpp"

namespace ats::world
{
    struct Transform2D
    {
        ats::math::Vector2 position{};
        double rotationRadians = 0.0;
        [[nodiscard]] ats::math::Vector2 FacingDirection() const
        {
            return {
                std::cos(rotationRadians),
                std::sin(rotationRadians)
            };
        }
    };
}