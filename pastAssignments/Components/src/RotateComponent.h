#pragma once
#include "Component.h"
#include "BodyComponent.h"

class RotateComponent : public Component {
public:
    RotateComponent(GameObject& parent, double rotationSpeed)
        : Component(parent), rotationSpeed(rotationSpeed) {}

    void update() override {
        auto body = parent().get<BodyComponent>();
        if (!body) return;

        // Increase angle each frame
        body->angle += rotationSpeed;

        // Keep angle in 0-360 range
        if (body->angle >= 360.0) {
            body->angle -= 360.0;
        }
        if (body->angle < 0.0) {
            body->angle += 360.0;
        }
    }

    void draw() override {
        // RotateComponent doesn't draw anything
    }

private:
    double rotationSpeed;  // Degrees per frame
};
