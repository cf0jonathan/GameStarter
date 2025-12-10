#pragma once
#include "Component.h"

// Simple lifetime component that marks the owner for deletion after a duration
class LifetimeComponent : public Component {
public:
    LifetimeComponent() = default;
    explicit LifetimeComponent(float seconds) : remaining(seconds) {}

    void setLifetime(float seconds) { remaining = seconds; }

    void update(float deltaTime) override;

private:
    float remaining = 1.0f; // seconds
};
