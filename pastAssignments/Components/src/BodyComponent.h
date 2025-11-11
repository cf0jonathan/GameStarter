#pragma once
#include "Component.h"

class BodyComponent : public Component {
public:
    BodyComponent(GameObject& owner, double x, double y)
        : Component(owner), x(x), y(y), vx(0), vy(0), angle(0) {}

    BodyComponent(GameObject& owner, double x, double y, double vx, double vy, double angle)
        : Component(owner), x(x), y(y), vx(vx), vy(vy), angle(angle) {}

    void update() override {
        // Apply velocity to position
        x += vx;
        y += vy;
    }

    void draw() override {
        // BodyComponent doesn't draw anything, but must implement this
    }

    // Public member variables for easy access (using camelCase)
    double x, y;        // Position
    double vx, vy;      // Velocity
    double angle;       // Rotation angle
};
