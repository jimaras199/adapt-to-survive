#pragma once

#include "ats/world/MovementConfig.hpp"
#include "ats/world/MovementInput.hpp"
#include "ats/world/MovementSystem.hpp"
#include "ats/world/Transform2D.hpp"
#include "ats/world/World.hpp"

namespace ats::world
{
    class EcsMovementSystem
    {
    public:
        void Update(World& world, double deltaTime)
        {
            auto& registry = world.GetRegistry();

            auto view = registry.view<
                Transform2D,
                const MovementInput,
                const MovementConfig
            >();

            for (auto entity : view)
            {
                auto& transform = view.get<Transform2D>(entity);
                const auto& input = view.get<const MovementInput>(entity);
                const auto& config = view.get<const MovementConfig>(entity);

                MovementSystem::Update(
                    transform,
                    input.localMovement,
                    input.rotationInput,
                    config.movementSpeed,
                    config.rotationSpeed,
                    deltaTime
                );
            }
        }
    };
}