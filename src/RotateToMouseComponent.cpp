#include "RotateToMouseComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "View.h"
#include <cmath>

void RotateToMouseComponent::init() {
    transform = owner->getComponent<TransformComponent>();
}

void RotateToMouseComponent::update(float deltaTime) {
    if (!transform) return;
    
    // Get mouse position in screen space
    int mouseScreenX = Engine::getMouseX();
    int mouseScreenY = Engine::getMouseY();
    
    // Convert screen to world using View helper
    float mouseWorldX, mouseWorldY;
    View::getInstance().screenToWorld(mouseScreenX, mouseScreenY, mouseWorldX, mouseWorldY);
    
    // Calculate angle from rocket to mouse
    float dx = mouseWorldX - transform->getX();
    float dy = mouseWorldY - transform->getY();
    
    // atan2 returns radians, convert to degrees
    // atan2(dy, dx) gives angle where 0 = right, 90 = down (SDL coordinate system)
    float angleRadians = std::atan2(dy, dx);
    float angleDegrees = angleRadians * 180.0f / 3.14159265f;
    
    transform->setRotation(angleDegrees);
}
