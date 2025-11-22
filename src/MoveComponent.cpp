#include "MoveComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "PhysicsBodyComponent.h"
#include "Engine.h"
#include <SDL2/SDL.h>
#include <cmath>

void MoveComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    physicsBody = owner->getComponent<PhysicsBodyComponent>();
}

void MoveComponent::update(float dt) {
    if (!transform || !physicsBody || !physicsBody->isInitialized()) return;

    // Update mouse button state
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    isMouseButtonDown = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    b2BodyId bodyId = physicsBody->getBodyId();

    // If not holding button, let damping handle slowdown
    if (!isMouseButtonDown) {
        return;
    }

    // Calculate forward direction based on current rotation
    // transform->getRotation() is in degrees, Box2D uses radians
    const float angleRadians = transform->getRotation() * 3.14159265f / 180.0f;
    const float dirX = std::cos(angleRadians);
    const float dirY = std::sin(angleRadians);

    // Apply force in forward direction (convert thrustForce from pixels to meters units)
    float forceMeters = thrustForce / PhysicsBodyComponent::PIXELS_PER_METER;
    b2Vec2 force = {dirX * forceMeters, dirY * forceMeters};
    b2Body_ApplyForceToCenter(bodyId, force, true);

    // Cap max speed
    b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    float maxSpeedMeters = maxSpeed / PhysicsBodyComponent::PIXELS_PER_METER;
    if (speed > maxSpeedMeters && speed > 0.0f) {
        float scale = maxSpeedMeters / speed;
        b2Vec2 clamped = {vel.x * scale, vel.y * scale};
        b2Body_SetLinearVelocity(bodyId, clamped);
    }
}
