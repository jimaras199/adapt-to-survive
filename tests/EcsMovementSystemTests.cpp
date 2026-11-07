#include <gtest/gtest.h>

#include "ats/world/EcsMovementSystem.hpp"
#include "ats/world/MovementConfig.hpp"
#include "ats/world/MovementInput.hpp"
#include "ats/world/Transform2D.hpp"
#include "ats/world/World.hpp"

using namespace ats::world;

TEST(EcsMovementSystemTests, EmptyWorldUpdatesSuccessfully)
{
    World world;
    EcsMovementSystem movementSystem;

    EXPECT_NO_THROW(movementSystem.Update(world, 0.02));
    EXPECT_EQ(world.EntityCount(), 0);
}

TEST(EcsMovementSystemTests, EntityWithRequiredComponentsMovesForward)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<Transform2D>(entity);

    registry.emplace<MovementInput>(
        entity,
        ats::math::Vector2{ 1.0, 0.0 },
        0.0
    );

    registry.emplace<MovementConfig>(
        entity,
        3.0,
        2.0
    );

    movementSystem.Update(world, 0.02);

    const auto& transform = registry.get<Transform2D>(entity);

    EXPECT_NEAR(transform.position.x, 0.06, 1e-9);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-9);
}

TEST(EcsMovementSystemTests, EntitiesMissingComponentsAreIgnored)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<Transform2D>(entity);

    registry.emplace<MovementInput>(
        entity,
        ats::math::Vector2{ 1.0, 0.0 },
        0.0
    );

    movementSystem.Update(world, 0.02);

    const auto& transform = registry.get<Transform2D>(entity);

    EXPECT_DOUBLE_EQ(transform.position.x, 0.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 0.0);
}

TEST(EcsMovementSystemTests, MovementInputPersistsBetweenUpdates)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<Transform2D>(entity);

    registry.emplace<MovementInput>(
        entity,
        ats::math::Vector2{ 1.0, 0.0 },
        0.0
    );

    registry.emplace<MovementConfig>(
        entity,
        3.0,
        2.0
    );

    movementSystem.Update(world, 0.02);
    movementSystem.Update(world, 0.02);

    const auto& transform = registry.get<Transform2D>(entity);
    const auto& input = registry.get<MovementInput>(entity);

    EXPECT_NEAR(transform.position.x, 0.12, 1e-9);
    EXPECT_DOUBLE_EQ(input.localMovement.x, 1.0);
    EXPECT_DOUBLE_EQ(input.localMovement.y, 0.0);
    EXPECT_DOUBLE_EQ(input.rotationInput, 0.0);
}

TEST(EcsMovementSystemTests, MultipleEntitiesMoveIndependently)
{
    World world;
    EcsMovementSystem movementSystem;

    auto& registry = world.GetRegistry();

    const auto entityA = world.CreateEntity();
    const auto entityB = world.CreateEntity();

    registry.emplace<Transform2D>(entityA);
    registry.emplace<MovementInput>(entityA, ats::math::Vector2{ 1.0, 0.0 }, 0.0);
    registry.emplace<MovementConfig>(entityA, 3.0, 2.0);

    registry.emplace<Transform2D>(entityB);
    registry.emplace<MovementInput>(entityB, ats::math::Vector2{ 0.0, 1.0 }, 0.0);
    registry.emplace<MovementConfig>(entityB, 1.5, 2.0);

    movementSystem.Update(world, 0.02);

    const auto& transformA = registry.get<Transform2D>(entityA);
    const auto& transformB = registry.get<Transform2D>(entityB);

    EXPECT_NEAR(transformA.position.x, 0.06, 1e-9);
    EXPECT_NEAR(transformA.position.y, 0.0, 1e-9);

    EXPECT_NEAR(transformB.position.x, 0.0, 1e-9);
    EXPECT_NEAR(transformB.position.y, 0.03, 1e-9);
}

TEST(EcsMovementSystemTests, RotationUpdatesOrientation)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<Transform2D>(entity);
    registry.emplace<MovementInput>(entity, ats::math::Vector2{ 0.0, 0.0 }, 1.0);
    registry.emplace<MovementConfig>(entity, 3.0, 2.0);

    movementSystem.Update(world, 0.02);

    const auto& transform = registry.get<Transform2D>(entity);

    EXPECT_NEAR(transform.rotationRadians, 0.04, 1e-9);
    EXPECT_NEAR(transform.position.x, 0.0, 1e-9);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-9);
}

TEST(EcsMovementSystemTests, NeutralInputDoesNotMoveEntity)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    auto& transform = registry.emplace<Transform2D>(entity);
    transform.position = { 5.0, 8.0 };
    transform.rotationRadians = 1.2;

    registry.emplace<MovementInput>(entity);
    registry.emplace<MovementConfig>(entity, 3.0, 2.0);

    movementSystem.Update(world, 0.02);

    EXPECT_DOUBLE_EQ(transform.position.x, 5.0);
    EXPECT_DOUBLE_EQ(transform.position.y, 8.0);
    EXPECT_DOUBLE_EQ(transform.rotationRadians, 1.2);
}

TEST(EcsMovementSystemTests, MovementInputCanChangeBetweenUpdates)
{
    World world;
    EcsMovementSystem movementSystem;

    const auto entity = world.CreateEntity();
    auto& registry = world.GetRegistry();

    registry.emplace<Transform2D>(entity);
    auto& input = registry.emplace<MovementInput>(
        entity, ats::math::Vector2{ 1.0, 0.0 }, 0.0
    );
    registry.emplace<MovementConfig>(entity, 3.0, 2.0);

    movementSystem.Update(world, 0.02);

    input.localMovement = { 0.0, 0.0 };
    input.rotationInput = 0.0;

    movementSystem.Update(world, 0.02);

    const auto& transform = registry.get<Transform2D>(entity);

    EXPECT_NEAR(transform.position.x, 0.06, 1e-9);
    EXPECT_NEAR(transform.position.y, 0.0, 1e-9);
    EXPECT_DOUBLE_EQ(transform.rotationRadians, 0.0);
}