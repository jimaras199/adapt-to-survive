#pragma once

#include <entt/entt.hpp>
#include <cmath>
#include <stdexcept>
#include "ats/world/WorldBounds.hpp"

namespace ats::simulation
{
    class Simulation;
}

namespace ats::world
{
    class World
    {
    public:

        explicit World(WorldBounds bounds = {})
            : bounds_(bounds)
        {
            if (!std::isfinite(bounds_.width) ||
                !std::isfinite(bounds_.height) ||
                bounds_.width <= 0.0 ||
                bounds_.height <= 0.0)
            {
                throw std::invalid_argument(
                    "World dimensions must be finite and positive"
                );
            }
        }

        const WorldBounds& GetBounds() const noexcept
        {
            return bounds_;
        }

        entt::entity CreateEntity()
        {
            return registry_.create();
        }

        void DestroyEntity(entt::entity entity)
        {
            registry_.destroy(entity);
        }

        bool IsValidEntity(entt::entity entity) const
        {
            return registry_.valid(entity);
        }

        std::size_t EntityCount() const
        {
            return registry_.view<entt::entity>().size();
        }

        entt::registry& GetRegistry() noexcept
        {
            return registry_;
        }

        const entt::registry& GetRegistry() const noexcept
        {
            return registry_;
        }

    private:
        WorldBounds bounds_;
        entt::registry registry_{};
    };
}