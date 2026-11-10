#pragma once

#include <cmath>
#include <stdexcept>

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/SurvivalDamageConfig.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

namespace ats::world
{
    class SurvivalDamageSystem
    {
    public:
        explicit SurvivalDamageSystem(
            SurvivalDamageConfig config = {}
        )
            : config_(config)
        {
            if (!std::isfinite(config_.damageRate) ||
                config_.damageRate < 0.0)
            {
                throw std::invalid_argument(
                    "Damage rate must be finite and nonnegative"
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

            auto view = registry.view<
                const SurvivalState,
                DamageAccumulator
            >();

            for (auto entity : view)
            {
                const auto& state =
                    view.get<const SurvivalState>(entity);

                auto& accumulator =
                    view.get<DamageAccumulator>(entity);

                int depletedNeeds = 0;

                if (state.food <= 0.0)
                {
                    ++depletedNeeds;
                }

                if (state.water <= 0.0)
                {
                    ++depletedNeeds;
                }

                const double damage =
                    depletedNeeds * config_.damageRate * deltaTime;

                accumulator.internalDamage += damage;
            }
        }

    private:
        SurvivalDamageConfig config_;
    };
}