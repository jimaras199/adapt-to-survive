#include <utility>

#include "ats/render/SfmlRenderer.hpp"
#include "ats/simulation/Simulation.hpp"
#include "ats/world/World.hpp"
#include "ats/world/WorldBounds.hpp"
#include "ats/world/Transform2D.hpp"
#include "ats/world/AgentTag.hpp"
#include "ats/world/Tree.hpp"
#include <entt/entity/fwd.hpp>
#include <ats/math/Vector2.hpp>

int main()
{
    ats::world::World world;

    const entt::entity agent = world.CreateEntity();
    world.GetRegistry().emplace<ats::world::AgentTag>(agent);
    world.GetRegistry().emplace<ats::world::Transform2D>(
        agent,
        ats::math::Vector2{ 250.0, 250.0 },
        0.0
    );

    const entt::entity tree1 = world.CreateEntity();

    world.GetRegistry().emplace<ats::world::Tree>(tree1);

    world.GetRegistry().emplace<ats::world::Transform2D>(
        tree1,
        ats::math::Vector2{ 100.0, 100.0 },
        0.0
    );

    const entt::entity tree2 = world.CreateEntity();

    world.GetRegistry().emplace<ats::world::Tree>(tree2);

    world.GetRegistry().emplace<ats::world::Transform2D>(
        tree2,
        ats::math::Vector2{ 400.0, 100.0 },
        0.0
    );

    const entt::entity tree3 = world.CreateEntity();

    world.GetRegistry().emplace<ats::world::Tree>(tree3);

    world.GetRegistry().emplace<ats::world::Transform2D>(
        tree3,
        ats::math::Vector2{ 100.0, 400.0 },
        0.0
    );

    ats::simulation::Simulation simulation(0.02, std::move(world));

    ats::render::SfmlRenderer renderer(
        simulation.GetWorld().GetBounds()
    );

    while (renderer.IsOpen())
    {
        renderer.ProcessEvents();

        if (!renderer.IsOpen())
        {
            break;
        }

        renderer.Render(simulation.GetWorld());
    }

    return 0;
}