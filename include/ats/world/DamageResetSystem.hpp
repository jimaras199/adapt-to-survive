#pragma once

#include "ats/world/DamageAccumulator.hpp"
#include "ats/world/World.hpp"

namespace ats::world
{
    class DamageResetSystem
    {
    public:
        void Update(World& world) const
        {
            auto& registry = world.GetRegistry();
            auto view = registry.view<DamageAccumulator>();

            for (auto entity : view)
            {
                auto& accumulator =
                    view.get<DamageAccumulator>(entity);

                accumulator.internalDamage = 0.0;
                accumulator.externalDamage = 0.0;
            }
        }
    };
}