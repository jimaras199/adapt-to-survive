#pragma once

#include <cmath>
#include <cstdint>
#include <stdexcept>

#include "ats/simulation/FixedStepRunner.hpp"
#include "ats/simulation/MovementCommand.hpp"
#include "ats/world/MovementSystem.hpp"
#include "ats/world/World.hpp"

namespace ats::simulation
{
    class Simulation
    {
    public:
        Simulation(
            double fixedDeltaTime,
            double movementSpeed,
            double rotationSpeed
        )
            : runner_(fixedDeltaTime),
            movementSpeed_(movementSpeed),
            rotationSpeed_(rotationSpeed)
        {
            if (!std::isfinite(movementSpeed_) || movementSpeed_ < 0.0)
            {
                throw std::invalid_argument(
                    "Movement speed must be finite and non-negative"
                );
            }

            if (!std::isfinite(rotationSpeed_) || rotationSpeed_ < 0.0)
            {
                throw std::invalid_argument(
                    "Rotation speed must be finite and non-negative"
                );
            }
        }

        void Step(const MovementCommand& command)
        {
            runner_.RunSteps(1, [this, &command](double deltaTime)
                {
                    ats::world::MovementSystem::Update(
                        world_.MutableAgentTransform(),
                        command.localMovement,
                        command.rotationInput,
                        movementSpeed_,
                        rotationSpeed_,
                        deltaTime
                    );
                });
        }

        const ats::world::World& GetWorld() const noexcept
        {
            return world_;
        }

        std::uint64_t CompletedSteps() const noexcept
        {
            return runner_.CompletedSteps();
        }

        double SimulatedTime() const noexcept
        {
            return runner_.SimulatedTime();
        }

    private:
        ats::world::World world_{};
        FixedStepRunner runner_;

        double movementSpeed_;
        double rotationSpeed_;
    };
}