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

    // If not holding button or within arrival radius, stop
    if (!isMouseButtonDown || dist < arrivalRadius) {
        b2Body_SetLinearVelocity(bodyId, {0.0f, 0.0f});
        return;
    }

    // Normalize direction and set velocity
    const float invDist = 1.0f / dist;
    const float dirX = dx * invDist;
    const float dirY = dy * invDist;

    // Convert velocity to physics units (pixels/sec to meters/sec)
    float velocityMetersPerSec = moveSpeed / PhysicsBodyComponent::PIXELS_PER_METER;
    b2Vec2 velocity = {dirX * velocityMetersPerSec, dirY * velocityMetersPerSec};
    
    b2Body_SetLinearVelocity(bodyId, velocity);
}
