#pragma once
#include "Component.h"

class TransformComponent;

/**
 * Moves the GameObject toward the mouse position when the left mouse button is held.
 * Kinematic movement with arrival radius to prevent overshoot.
 * Design note: When Box2D is added, swap direct Transform writes for body.SetLinearVelocity.
 */
class MoveToMouseComponent : public Component {
public:
    MoveToMouseComponent() = default;
    ~MoveToMouseComponent() override = default;

    void init() override;
    void update(float dt) override;

    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setArrivalRadius(float radius) { arrivalRadius = radius; }

private:
    float moveSpeed = 200.0f;       // units per second
    float arrivalRadius = 5.0f;     // stop moving when closer than this distance
    bool isMouseButtonDown = false; // track left mouse button state

    TransformComponent* transform = nullptr;
};
