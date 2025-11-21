#pragma once
#include "Component.h"

class TransformComponent;
class PhysicsBodyComponent;

/**
 * Moves the GameObject toward the mouse position when the left mouse button is held.
 * Uses velocity-based physics movement via Box2D body.
 */
class MoveToMouseComponent : public Component {
public:
    MoveToMouseComponent() = default;
    ~MoveToMouseComponent() override = default;

    void init() override;
    void update(float dt) override;

    void setMoveSpeed(float speed) { moveSpeed = speed; } // legacy (interpreted as maxSpeed if provided)
    void setThrustForce(float force) { thrustForce = force; }
    void setMaxSpeed(float ms) { maxSpeed = ms; }
    void setArrivalRadius(float radius) { arrivalRadius = radius; }

private:
    float moveSpeed = 200.0f;       // retained for backward compatibility
    float thrustForce = 20.0f;      // force applied toward mouse (pixels * mass per second equivalent)
    float maxSpeed = 400.0f;        // maximum linear speed in pixels/sec
    float arrivalRadius = 5.0f;     // stop moving when closer than this distance
    bool isMouseButtonDown = false; // track left mouse button state

    TransformComponent* transform = nullptr;
    PhysicsBodyComponent* physicsBody = nullptr;
};
