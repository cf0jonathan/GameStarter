#pragma once
#include "Component.h"
#include "BodyComponent.h"
#include "Input.h"
#include <SDL2/SDL.h>
#include <functional>

class ControllerComponent : public Component {
public:
    ControllerComponent(GameObject& parent, double speed = 5.0)
        : Component(parent), speed(speed), rotationSpeed(3.0), lastSpacePressed(false) {}

    ControllerComponent(GameObject& parent, double speed, double rotationSpeed)
        : Component(parent), speed(speed), rotationSpeed(rotationSpeed), lastSpacePressed(false) {}

    // Set callback for when projectile should be spawned
    void setProjectileSpawnCallback(std::function<void(double, double, double)> callback) {
        spawnProjectile = callback;
    }

    void update() override {
        auto body = parent().get<BodyComponent>();
        if (!body) return;

        // Reset velocity
        body->vx = 0;
        body->vy = 0;

        // Check movement input and set velocity
        if (Input::isKeyDown(SDLK_w) || Input::isKeyDown(SDLK_UP)) {
            body->vy = -speed;
        }
        if (Input::isKeyDown(SDLK_s) || Input::isKeyDown(SDLK_DOWN)) {
            body->vy = speed;
        }
        if (Input::isKeyDown(SDLK_a) || Input::isKeyDown(SDLK_LEFT)) {
            body->vx = -speed;
        }
        if (Input::isKeyDown(SDLK_d) || Input::isKeyDown(SDLK_RIGHT)) {
            body->vx = speed;
        }

        // Check rotation input
        if (Input::isKeyDown(SDLK_q)) {
            body->angle -= rotationSpeed;  // Rotate counter-clockwise
        }
        if (Input::isKeyDown(SDLK_e)) {
            body->angle += rotationSpeed;   // Rotate clockwise
        }

        // Keep angle in 0-360 range
        if (body->angle >= 360.0) {
            body->angle -= 360.0;
        }
        if (body->angle < 0.0) {
            body->angle += 360.0;
        }

        // Handle projectile shooting (space bar)
        bool spacePressed = Input::isKeyDown(SDLK_SPACE);
        if (spacePressed && !lastSpacePressed && spawnProjectile) {
            // Spawn projectile at player position, moving in player's facing direction
            spawnProjectile(body->x + 32, body->y + 32, body->angle);  // +32 to center of sprite
        }
        lastSpacePressed = spacePressed;
    }

    void draw() override {
        // ControllerComponent doesn't draw anything
    }

private:
    double speed;
    double rotationSpeed;
    bool lastSpacePressed;  // To detect space key press (not hold)
    std::function<void(double, double, double)> spawnProjectile;  // Callback to spawn projectile
};
