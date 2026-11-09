#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "ats/world/SurvivalDepletionConfig.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

namespace ats::world
{
    class SurvivalDepletionSystem
    {
    public:
        explicit SurvivalDepletionSystem(
            SurvivalDepletionConfig config = {}
        )
            : config_(config)
        {
            if (!std::isfinite(config_.foodRate) ||
                !std::isfinite(config_.waterRate) ||
                config_.foodRate < 0.0 ||
                config_.waterRate < 0.0)
            {
                throw std::invalid_argument(
                    "Depletion rates must be finite and nonnegative"
                );
            }
        }

        void Update(World& world, double deltaTime) const
        {
            if (!std::isfinite(deltaTime) || deltaTime < 0.0)
            {
                throw std::invalid_argument(
                    "Delta time must be finite and nonnegative"
                );
            }

            auto& registry = world.GetRegistry();
            auto view = registry.view<SurvivalState>();

            for (auto entity : view)
            {
                auto& state = view.get<SurvivalState>(entity);

                state.food = std::clamp(
                    state.food - config_.foodRate * deltaTime,
                    0.0,
                    100.0
                );

                state.water = std::clamp(
                    state.water - config_.waterRate * deltaTime,
                    0.0,
                    100.0
                );
            }
        }

    private:
        SurvivalDepletionConfig config_;
    };
}