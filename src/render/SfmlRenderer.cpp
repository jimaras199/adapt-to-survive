#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "ats/render/SfmlRenderer.hpp"
#include "ats/world/World.hpp"
#include "ats/world/AgentTag.hpp"
#include "ats/world/Transform2D.hpp"

namespace ats::render
{
    SfmlRenderer::SfmlRenderer(const ats::world::WorldBounds& bounds)
        : m_window(
            sf::VideoMode({ 800, 600 }),
            "Adapt to Survive"
        ),
        m_bounds(bounds),
        m_worldView(
            sf::FloatRect(
                { 0.0f, 0.0f },
            {
                static_cast<float>(bounds.width),
                static_cast<float>(bounds.height)
            }
            )
        )
    {
        m_window.setFramerateLimit(60);
        UpdateView();
    }

    void SfmlRenderer::Render(const ats::world::World& world)
    {
        m_window.clear(sf::Color(0, 0, 0));
        m_window.setView(m_worldView);

        DrawGround();
        DrawAgents(world);

        m_window.display();
    }

    void SfmlRenderer::DrawGround()
    {
        sf::RectangleShape boundary({
        static_cast<float>(m_bounds.width),
        static_cast<float>(m_bounds.height)
            });

        boundary.setPosition({ 0.0f, 0.0f });
        boundary.setFillColor(sf::Color(125, 175, 105));

        m_window.draw(boundary);
    }

    void SfmlRenderer::DrawAgents(const ats::world::World& world)
    {
        const auto& registry = world.GetRegistry();

        auto view = registry.view<
            const ats::world::AgentTag,
            const ats::world::Transform2D
        >();

        constexpr float agentRadius = 10.0f;

        for (const auto entity : view)
        {
            const auto& transform =
                view.get<const ats::world::Transform2D>(entity);

            sf::CircleShape circle(agentRadius);

            circle.setOrigin({ agentRadius, agentRadius });

            circle.setPosition({
                static_cast<float>(transform.position.x),
                static_cast<float>(transform.position.y)
                });

            circle.setFillColor(sf::Color::Blue);

            m_window.draw(circle);
        }
    }

    void SfmlRenderer::UpdateView()
    {
        const sf::Vector2u windowSize = m_window.getSize();

        if (windowSize.x == 0 || windowSize.y == 0)
        {
            return;
        }

        const float worldWidth = static_cast<float>(m_bounds.width);
        const float worldHeight = static_cast<float>(m_bounds.height);

        const float windowWidth = static_cast<float>(windowSize.x);
        const float windowHeight = static_cast<float>(windowSize.y);

        constexpr float viewportPadding = 0.02f;

        const float availableWidth =
            windowWidth * (1.0f - 2.0f * viewportPadding);

        const float availableHeight =
            windowHeight * (1.0f - 2.0f * viewportPadding);

        const float scale = std::min(
            availableWidth / worldWidth,
            availableHeight / worldHeight
        );

        const float viewportWidth = (worldWidth * scale) / windowWidth;
        const float viewportHeight = (worldHeight * scale) / windowHeight;

        m_worldView.setViewport(sf::FloatRect(
            {
                (1.0f - viewportWidth) / 2.0f,
                (1.0f - viewportHeight) / 2.0f
            },
            { viewportWidth, viewportHeight }
        ));
    }

    void SfmlRenderer::ProcessEvents()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            }
            else if (event->is<sf::Event::Resized>())
            {
                UpdateView();
            }
        }
    }

    bool SfmlRenderer::IsOpen() const
    {
        return m_window.isOpen();
    }
}