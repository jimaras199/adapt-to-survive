#include <utility>

#include "ats/simulation/Simulation.hpp"
#include "ats/world/World.hpp"

int main()
{
    ats::world::World world;

    ats::simulation::Simulation simulation(
        0.02,
        std::move(world)
    );

    for (int i = 0; i < 100; ++i)
    {
        simulation.Tick();
    }

    return 0;
}