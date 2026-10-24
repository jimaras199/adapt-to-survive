
#pragma once

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "ats/math/Vector2.hpp"
#include "ats/world/Transform2D.hpp"

namespace ats::world
{
    class MovementSystem
    {
    public:
        static void Update(
            Transform2D& transform,
            const ats::math::Vector2& localMovement,
            double rotationInput,
            double movementSpeed,
            double rotationSpeed,
            double deltaTime)
        {
            if (!std::isfinite(localMovement.x) ||
                !std::isfinite(localMovement.y) ||
                !std::isfinite(rotationInput) ||
                !std::isfinite(movementSpeed) ||
                !std::isfinite(rotationSpeed) ||
                !std::isfinite(deltaTime))
            {
                throw std::invalid_argument("Movement inputs must be finite");
            }

            if (movementSpeed < 0.0 ||
                rotationSpeed < 0.0 ||
                deltaTime < 0.0)
            {
                throw std::invalid_argument("Speed and delta time cannot be negative");
            }

            ats::math::Vector2 movement{
                std::clamp(localMovement.x, -1.0, 1.0),
                std::clamp(localMovement.y, -1.0, 1.0)
            };

            const double magnitude = std::sqrt(
                movement.x * movement.x +
                movement.y * movement.y
            );

            if (magnitude > 1.0)
            {
                movement = movement * (1.0 / magnitude);
            }

            const double clampedRotation =
                std::clamp(rotationInput, -1.0, 1.0);

            const double angle = transform.rotationRadians;

            const double cosine = std::cos(angle);
            const double sine = std::sin(angle);

            const ats::math::Vector2 worldMovement{
                movement.x * cosine - movement.y * sine,
                movement.x * sine + movement.y * cosine
            };

            transform.position = transform.position +
                worldMovement * (movementSpeed * deltaTime);

            transform.rotationRadians +=
                clampedRotation * rotationSpeed * deltaTime;
        }
    };
}
