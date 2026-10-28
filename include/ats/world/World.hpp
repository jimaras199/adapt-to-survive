#pragma once

#include "ats/world/Agent.hpp"

namespace ats::simulation
{
    class Simulation;
}

namespace ats::world
{
    class World
    {
    public:
        const Agent& GetAgent() const noexcept
        {
            return agent_;
        }

    private:
        Agent agent_{};

        Transform2D& MutableAgentTransform() noexcept
        {
            return agent_.transform;
        }

        friend class ats::simulation::Simulation;
    };
}