#pragma once
#include "Component.h"
#include "BodyComponent.h"

class SlideComponent : public Component {
public:
    SlideComponent(GameObject& parent, double speed, double leftBound, double rightBound)
        : Component(parent), speed(speed), leftBound(leftBound), rightBound(rightBound), movingRight(true) {}

    void update() override {
        auto body = parent().get<BodyComponent>();
        if (!body) return;

        // Check boundaries and reverse direction
        if (body->x >= rightBound) {
            movingRight = false;
        }
        if (body->x <= leftBound) {
            movingRight = true;
        }

        // Set velocity based on direction
        if (movingRight) {
            body->vx = speed;
        } else {
            body->vx = -speed;
        }
    }

    void draw() override {
        // SlideComponent doesn't draw anything
    }

private:
    double speed;
    double leftBound;
    double rightBound;
    bool movingRight;
};
