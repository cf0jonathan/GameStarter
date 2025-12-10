#include "MoveComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "PhysicsBodyComponent.h"
#include "InputComponent.h"
#include "Engine.h"
#include "AssetManager.h"
#include <SDL2/SDL.h>
#include <cmath>

void MoveComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    physicsBody = owner->getComponent<PhysicsBodyComponent>();
    input = owner->getComponent<InputComponent>();
}

void MoveComponent::update(float dt) {
    if (!transform || !physicsBody || !physicsBody->isInitialized()) return;
    
    // Debug: F2 toggles spawn/despawn of an asteroid ~50px to the left of player
    const Uint8* keyStates = SDL_GetKeyboardState(nullptr);
    bool isF2Down = keyStates[SDL_SCANCODE_F2];
    if (isF2Down && !wasF2Down) { // Key just pressed
        if (debugAsteroid == nullptr) {
            float ax = transform->getX() - 150.0f;
            float ay = transform->getY();
            debugAsteroid = Engine::getInstance().spawnAsteroid(ax, ay, 85.0f);
        } else {
            Engine::getInstance().removeGameObject(debugAsteroid);
            debugAsteroid = nullptr;
        }
    }
    wasF2Down = isF2Down;

    b2BodyId bodyId = physicsBody->getBodyId();

    bool isThrusting = input && input->isLeftMouseDown();
    
    // Handle rocket sound
    if (isThrusting && !wasThrusting) {
        // Start playing rocket sound loop
        rocketSoundChannel = AssetManager::getInstance().playSoundLoop("rocket");
    } else if (!isThrusting && wasThrusting) {
        // Stop rocket sound
        AssetManager::getInstance().stopSound(rocketSoundChannel);
        rocketSoundChannel = -1;
    }
    wasThrusting = isThrusting;

    // If not holding button, let damping handle slowdown
    if (!isThrusting) {
        return;
    }

    // Calculate forward direction based on current rotation
    // transform->getRotation() is in degrees, Box2D uses radians
    const float angleRadians = transform->getRotation() * 3.14159265f / 180.0f;
    const float dirX = std::cos(angleRadians);
    const float dirY = std::sin(angleRadians);

    // Apply force in forward direction (convert thrustForce from pixels to meters units)
    float forceMeters = thrustForce / PhysicsBodyComponent::PIXELS_PER_METER;
    b2Vec2 force = {dirX * forceMeters, dirY * forceMeters};
    b2Body_ApplyForceToCenter(bodyId, force, true);

    // Cap max speed
    b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    float maxSpeedMeters = maxSpeed / PhysicsBodyComponent::PIXELS_PER_METER;
    if (speed > maxSpeedMeters && speed > 0.0f) {
        float scale = maxSpeedMeters / speed;
        b2Vec2 clamped = {vel.x * scale, vel.y * scale};
        b2Body_SetLinearVelocity(bodyId, clamped);
    }
}
