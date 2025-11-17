#include "MoveToMouseComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "View.h"
#include <SDL2/SDL.h>
#include <cmath>

void MoveToMouseComponent::init() {
    transform = owner->getComponent<TransformComponent>();
}

void MoveToMouseComponent::update(float dt) {
    if (!transform) return;

    // Update mouse button state from SDL events
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    isMouseButtonDown = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    // Only move when left mouse button is held
    if (!isMouseButtonDown) {
        return;
    }

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

    // Arrival: stop if within arrival radius
    if (dist < arrivalRadius) {
        return;
    }

    // Normalize direction
    const float invDist = 1.0f / dist;
    const float dirX = dx * invDist;
    const float dirY = dy * invDist;

    // Calculate step size with arrival clamp
    const float stepSize = std::min(moveSpeed * dt, dist - arrivalRadius);

    // Update position
    transform->setPosition(posX + dirX * stepSize, posY + dirY * stepSize);
}
