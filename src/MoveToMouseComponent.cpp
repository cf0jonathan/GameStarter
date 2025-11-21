#include "MoveToMouseComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "PhysicsBodyComponent.h"
#include "Engine.h"
#include "View.h"
#include <SDL2/SDL.h>
#include <cmath>

void MoveToMouseComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    physicsBody = owner->getComponent<PhysicsBodyComponent>();
}

void MoveToMouseComponent::update(float dt) {
    if (!transform || !physicsBody || !physicsBody->isInitialized()) return;

    // Update mouse button state
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    isMouseButtonDown = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    // Get mouse position in world space
    int mouseScreenX = Engine::getMouseX();
    int mouseScreenY = Engine::getMouseY();
    float mouseWorldX, mouseWorldY;
    View::getInstance().screenToWorld(mouseScreenX, mouseScreenY, mouseWorldX, mouseWorldY);

    // Get current position
    const float posX = transform->getX();
    const float posY = transform->getY();

    // Calculate direction vector to mouse
    const float dx = mouseWorldX - posX;
    const float dy = mouseWorldY - posY;
    const float distSquared = dx * dx + dy * dy;
    const float dist = std::sqrt(distSquared);

    b2BodyId bodyId = physicsBody->getBodyId();

    // If not holding button or within arrival radius, apply damping (let physics slow naturally)
    if (!isMouseButtonDown || dist < arrivalRadius) {
        return; // do nothing; damping handles slowdown
    }

    // Normalize direction
    const float invDist = 1.0f / dist;
    const float dirX = dx * invDist;
    const float dirY = dy * invDist;

    // Apply force toward mouse (convert thrustForce from pixels to meters units)
    float forceMeters = thrustForce / PhysicsBodyComponent::PIXELS_PER_METER; // simple scaling
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
