#pragma once

#include <cstdint>
#include <utility>

#include "ats/simulation/FixedStepRunner.hpp"
#include "ats/simulation/SystemScheduler.hpp"
#include "ats/world/World.hpp"

namespace ats::simulation
{
    class Simulation
    {
    public:
        explicit Simulation(double fixedDeltaTime, ats::world::World world)
            : world_(std::move(world)),
            runner_(fixedDeltaTime)
        {
        }

        void AddSystem(SystemScheduler::SystemCallback system)
        {
            scheduler_.AddSystem(std::move(system));
        }

        void Tick()
        {
            runner_.RunSteps(1, [this](double deltaTime)
            {
                scheduler_.Update(world_, deltaTime);
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
        ats::world::World world_;
        FixedStepRunner runner_;
        SystemScheduler scheduler_;
    };
}