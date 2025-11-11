#pragma once
#include "Component.h"
#include "BodyComponent.h"

// Forward declaration to avoid circular include
class Engine;

class ProjectileComponent : public Component {
public:
    ProjectileComponent(GameObject& parent, double speed, double directionAngle, double screenWidth = 800, double screenHeight = 600)
        : Component(parent), speed(speed), directionAngle(directionAngle), 
          screenWidth(screenWidth), screenHeight(screenHeight), isOffScreen(false) {
        
        // Calculate velocity based on direction angle
        auto body = parent.get<BodyComponent>();
        if (body) {
            // Convert angle to radians for math functions
            double radians = directionAngle * (3.14159 / 180.0);
            body->vx = speed * cos(radians);
            body->vy = speed * sin(radians);
        }
    }

    void update() override {
        auto body = parent().get<BodyComponent>();
        if (!body) {
            isOffScreen = true;  // Mark for destruction if no body component
            return;
        }

        // Check if projectile is off-screen
        if (body->x < -64 || body->x > screenWidth + 64 || 
            body->y < -64 || body->y > screenHeight + 64) {
            isOffScreen = true;
        }
    }

    void draw() override {
        // ProjectileComponent doesn't draw, SpriteComponent handles that
    }

    bool shouldDestroy() const {
        return isOffScreen;
    }

private:
    double speed;
    double directionAngle;
    double screenWidth;
    double screenHeight;
    bool isOffScreen;
};