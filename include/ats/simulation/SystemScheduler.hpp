#pragma once

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "ats/world/World.hpp"

namespace ats::simulation
{
    class SystemScheduler
    {
    public:
        using SystemCallback =
            std::function<void(ats::world::World&, double)>;

        void AddSystem(SystemCallback system)
        {
            if (updating_)
            {
                throw std::logic_error(
                    "Cannot register systems during an update"
                );
            }

            if (!system)
            {
                throw std::invalid_argument(
                    "Cannot register an empty system"
                );
            }

            systems_.push_back(std::move(system));
        }

        void Update(ats::world::World& world, double deltaTime)
        {
            if (updating_)
            {
                throw std::logic_error(
                    "Recursive scheduler updates are not allowed"
                );
            }

            updating_ = true;

            try
            {
                for (auto& system : systems_)
                {
                    system(world, deltaTime);
                }
            }
            catch (...)
            {
                updating_ = false;
                throw;
            }

            updating_ = false;
        }

    private:
        std::vector<SystemCallback> systems_;
        bool updating_{ false };
    };
}