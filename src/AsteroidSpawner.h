#pragma once
#include "Component.h"

class TransformComponent;

class AsteroidSpawner : public Component {
public:
    AsteroidSpawner() = default;
    ~AsteroidSpawner() override = default;

    void init() override;
    void update(float deltaTime) override;

    // Configuration
    void setSpawnBuffer(float buffer) { spawnBuffer = buffer; }
    void setCleanupDistance(float dist) { cleanupDistance = dist; }
    void setSpawnDensity(float count) { spawnDensityPerDirection = count; }  // Fractional: 0.25, 0.5, 1.0, etc.
    void setAsteroidSizeMin(float s) { asteroidSizeMin = s; }
    void setAsteroidSizeMax(float s) { asteroidSizeMax = s; }

private:
    TransformComponent* transform = nullptr;

    // Spawn/despawn parameters
    float spawnBuffer = 500.0f;            // spawn asteroids this far beyond screen edge
    float cleanupDistance = 1500.0f;       // despawn asteroids this far from player
    float spawnDensityPerDirection = 0.25f; // fractional asteroids per direction per spawn trigger
    float asteroidSizeMin = 60.0f;
    float asteroidSizeMax = 120.0f;

    // Tracking
    float lastSpawnX = 0.0f;               // last spawn x position (to avoid duplicates)
    float spawnAccumulator = 0.0f;         // accumulator for fractional spawns per direction
};
