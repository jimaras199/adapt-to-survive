#pragma once

#include <entt/entt.hpp>

namespace ats::simulation
{
    class Simulation;
}

namespace ats::world
{
    class World
    {
    public:
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
        entt::registry registry_{};
    };
}