#pragma once
#include "Component.h"

class TransformComponent;
class PhysicsBodyComponent;

/**
 * Applies forward thrust to the GameObject when input is active.
 * Thrust direction is based on the current facing direction (rotation).
 * Uses force-based physics movement via Box2D body.
 */
class MoveComponent : public Component {
public:
    MoveComponent() = default;
    ~MoveComponent() override = default;

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
    
    // Debug spawn
    class GameObject* debugAsteroid = nullptr;
    bool wasF2Down = false;

    TransformComponent* transform = nullptr;
    PhysicsBodyComponent* physicsBody = nullptr;
};
