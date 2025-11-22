#include "RotateToMouseComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "PhysicsBodyComponent.h"
#include "Engine.h"
#include "View.h"
#include <cmath>

void RotateToMouseComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    physicsBody = owner->getComponent<PhysicsBodyComponent>();
}

void RotateToMouseComponent::update(float deltaTime) {
    if (!transform) return;
    
    // Get mouse position in screen space
    int mouseScreenX = Engine::getMouseX();
    int mouseScreenY = Engine::getMouseY();
    
    // Convert screen to world using View helper
    float mouseWorldX, mouseWorldY;
    View::getInstance().screenToWorld(mouseScreenX, mouseScreenY, mouseWorldX, mouseWorldY);
    
    // Calculate desired angle to mouse
    float dx = mouseWorldX - transform->getX();
    float dy = mouseWorldY - transform->getY();
    float targetAngleDegrees = std::atan2(dy, dx) * 180.0f / 3.14159265f;

    float currentAngleDegrees = transform->getRotation();
    float angleDelta = targetAngleDegrees - currentAngleDegrees;

    // Normalize angle delta to [-180, 180]
    while (angleDelta > 180.0f) angleDelta -= 360.0f;
    while (angleDelta < -180.0f) angleDelta += 360.0f;

    float absDelta = std::abs(angleDelta);

    // Dead zone snap
    if (absDelta < deadZone) {
        currentAngularSpeed = 0.0f;
        transform->setRotation(targetAngleDegrees);
        if (physicsBody && physicsBody->isInitialized()) {
            b2BodyId bodyId = physicsBody->getBodyId();
            float angleRadians = targetAngleDegrees * 3.14159265f / 180.0f;
            b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), b2MakeRot(angleRadians));
        }
        return;
    }

    // Determine target speed with easing inside slowRadius
    float targetSpeed;
    if (absDelta > slowRadius) {
        targetSpeed = maxAngularSpeed;
    } else {
        float ratio = absDelta / slowRadius; // 0..1
        // Ease (pow) for softer slow-down; ensure ratio stays in [0,1]
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        targetSpeed = maxAngularSpeed * std::pow(ratio, easeExponent);
        // Guarantee a minimal speed to prevent stall far from deadZone
        if (targetSpeed < 10.0f) targetSpeed = 10.0f;
    }
    // Apply direction sign
    targetSpeed *= (angleDelta > 0.0f) ? 1.0f : -1.0f;

    // Handle direction flip (sign change) aggressively
    if (currentAngularSpeed != 0.0f && (currentAngularSpeed * targetSpeed) < 0.0f) {
        // Apply strong braking
        float brakeAmount = deceleration * 2.0f * deltaTime;
        if (std::abs(currentAngularSpeed) <= brakeAmount) {
            currentAngularSpeed = 0.0f; // fully stop
        } else {
            currentAngularSpeed -= brakeAmount * (currentAngularSpeed > 0.0f ? 1.0f : -1.0f);
        }
    }

    // Accelerate or decelerate toward target speed
    if (currentAngularSpeed < targetSpeed) {
        currentAngularSpeed += acceleration * deltaTime;
        if (currentAngularSpeed > targetSpeed) currentAngularSpeed = targetSpeed;
    } else if (currentAngularSpeed > targetSpeed) {
        currentAngularSpeed -= deceleration * deltaTime;
        if (currentAngularSpeed < targetSpeed) currentAngularSpeed = targetSpeed;
    }

    // Clamp to maxAngularSpeed
    if (std::abs(currentAngularSpeed) > maxAngularSpeed) {
        currentAngularSpeed = (currentAngularSpeed > 0.0f ? maxAngularSpeed : -maxAngularSpeed);
    }

    // Advance rotation
    currentAngleDegrees += currentAngularSpeed * deltaTime;

    // Recompute remaining delta for possible overshoot snap
    float newDelta = targetAngleDegrees - currentAngleDegrees;
    while (newDelta > 180.0f) newDelta -= 360.0f;
    while (newDelta < -180.0f) newDelta += 360.0f;
    if (std::abs(newDelta) < deadZone) {
        currentAngleDegrees = targetAngleDegrees;
        currentAngularSpeed = 0.0f;
    }

    transform->setRotation(currentAngleDegrees);

    if (physicsBody && physicsBody->isInitialized()) {
        b2BodyId bodyId = physicsBody->getBodyId();
        float angleRadians = currentAngleDegrees * 3.14159265f / 180.0f;
        b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), b2MakeRot(angleRadians));
    }
}
