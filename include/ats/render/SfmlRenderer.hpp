#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "ats/world/World.hpp"
#include "ats/world/WorldBounds.hpp"

namespace ats::render
{
    class SfmlRenderer
    {
    public:
        explicit SfmlRenderer(const ats::world::WorldBounds& bounds);

        bool IsOpen() const;

        void ProcessEvents();
        void Render(const ats::world::World& world);

    private:
        void DrawGround();
        void DrawAgents(const ats::world::World& world);
        void UpdateView();

        sf::RenderWindow m_window;
        ats::world::WorldBounds m_bounds;
        sf::View m_worldView;
    };
}