#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/SurvivalState.hpp"
#include "ats/world/World.hpp"

namespace ats::world
{
    class HealthApplicationSystem
    {
    public:
        void Update(World& world) const
        {
            auto& registry = world.GetRegistry();

            auto view = registry.view<
                SurvivalState,
                const DamageAccumulator
            >();

            for (auto entity : view)
            {
                auto& state = view.get<SurvivalState>(entity);

                const auto& accumulator = view.get<const DamageAccumulator>(entity);

                if (!std::isfinite(accumulator.internalDamage) ||
                    !std::isfinite(accumulator.externalDamage) ||
                    accumulator.internalDamage < 0.0 ||
                    accumulator.externalDamage < 0.0)
                {
                    throw std::invalid_argument("Accumulated damage must be finite and nonnegative");
                }

                const double totalDamage = accumulator.internalDamage + accumulator.externalDamage;

                if (!std::isfinite(totalDamage))
                {
                    throw std::overflow_error(
                        "Total accumulated damage must be finite"
                    );
                }

                state.health = std::clamp(state.health - totalDamage, 0.0, 100.0);
            }
        }
    };
}