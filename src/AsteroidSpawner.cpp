#include "AsteroidSpawner.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include <cmath>
#include <cstdlib>
#include <vector>

void AsteroidSpawner::init() {
    transform = owner->getComponent<TransformComponent>();
    lastSpawnX = transform ? transform->getX() : 0.0f;
}

void AsteroidSpawner::update(float deltaTime) {
    if (!transform) return;

    float playerX = transform->getX();
    float playerY = transform->getY();

    // Get window dimensions for spawn edge calculation
    int windowWidth = Engine::getInstance().getWindowWidth();
    int windowHeight = Engine::getInstance().getWindowHeight();
    float halfWidth = windowWidth / 2.0f;
    float halfHeight = windowHeight / 2.0f;

    // Check if we should spawn (only if player moved 100+ pixels since last spawn)
    if (playerX - lastSpawnX > 100.0f) {
        // Add to accumulator based on spawn density
        spawnAccumulator += spawnDensityPerDirection;
        int asteroidsToSpawn = static_cast<int>(spawnAccumulator);
        spawnAccumulator -= asteroidsToSpawn;  // Keep the fractional part
        
        // Get the player's facing direction (rotation in degrees)
        float playerRotation = transform->getRotation();
        // Convert to radians
        float playerAngleRad = playerRotation * 3.14159265f / 180.0f;
        
        // Spawn asteroids in a 180-degree arc ahead of the rocket
        for (int count = 0; count < asteroidsToSpawn; ++count) {
            float size = asteroidSizeMin + (static_cast<float>(rand()) / RAND_MAX) * (asteroidSizeMax - asteroidSizeMin);
            
            // Generate a random angle within 180 degrees centered on the player's facing direction
            // Range: playerAngleRad ± 90 degrees (so 180 degree arc total)
            float angleOffset = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 3.14159265f;  // -90 to +90 degrees in radians
            float spawnAngle = playerAngleRad + angleOffset;
            
            // Calculate spawn distance (at the edge of the spawn buffer)
            float spawnDistance = std::max(halfWidth, halfHeight) + spawnBuffer;
            
            // Calculate spawn position using angle
            float spawnX = playerX + std::cos(spawnAngle) * spawnDistance;
            float spawnY = playerY + std::sin(spawnAngle) * spawnDistance;
            
            Engine::getInstance().spawnAsteroid(spawnX, spawnY, size);
        }
        
        lastSpawnX = playerX;
    }

    // Cleanup distant asteroids
    // Create a snapshot of asteroids first to avoid iterator invalidation
    std::vector<GameObject*> asteroidsToCheck;
    for (auto& obj : Engine::getInstance().getGameObjects()) {
        if (obj && obj->hasTag("asteroid")) {
            asteroidsToCheck.push_back(obj.get());
        }
    }
    
    // Now check distances and mark for deletion
    for (GameObject* astPtr : asteroidsToCheck) {
        auto* astTransform = astPtr->getComponent<TransformComponent>();
        if (astTransform) {
            float dx = astTransform->getX() - playerX;
            float dy = astTransform->getY() - playerY;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > cleanupDistance) {
                astPtr->markForDeletion();
            }
        }
    }
}
