#pragma once

#include "ats/math/Vector2.hpp"

namespace ats::simulation
{
    struct MovementCommand
    {
        ats::math::Vector2 localMovement{};
        double rotationInput{ 0.0 };
    };
}