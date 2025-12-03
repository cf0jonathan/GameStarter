#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "InputComponent.h"
#include "ParticleSystem.h"
#include <cmath>
#include <cstdlib>

void ParticleEmitterComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    input = owner->getComponent<InputComponent>();
}

void ParticleEmitterComponent::update(float deltaTime) {
    if (!transform || !isActive) return;
    
    if (emitterType == EmitterType::Continuous) {
        // Check if mouse button condition is met
        if (requireMouseButton && (!input || !input->isLeftMouseDown())) {
            return;  // Don't emit if button not held
        }
        
        // Accumulate time and spawn particles at the specified rate
        accumulator += deltaTime;
        float spawnInterval = 1.0f / spawnRate;
        
        while (accumulator >= spawnInterval) {
            accumulator -= spawnInterval;
            
            // Get spawn position (transform position + offset rotated by facing direction)
            float rotation = transform->getRotation() * 3.14159265f / 180.0f;
            float spawnX = transform->getX() + 
                          (offsetX * std::cos(rotation) - offsetY * std::sin(rotation));
            float spawnY = transform->getY() + 
                          (offsetX * std::sin(rotation) + offsetY * std::cos(rotation));
            
            // Calculate base direction (facing direction + offset)
            float baseAngle = rotation + directionOffset * 3.14159265f / 180.0f;
            
            // Add random spread within cone
            float spreadRad = spreadAngle * 3.14159265f / 180.0f;
            float randomSpread = ((static_cast<float>(rand()) / RAND_MAX) * spreadRad) - (spreadRad / 2.0f);
            float finalAngle = baseAngle + randomSpread;
            
            // Random speed within range
            float speed = speedMin + (static_cast<float>(rand()) / RAND_MAX) * (speedMax - speedMin);
            
            // Create particle
            Particle p;
            p.x = spawnX;
            p.y = spawnY;
            p.vx = std::cos(finalAngle) * speed;
            p.vy = std::sin(finalAngle) * speed;
            p.lifetime = lifetime + (static_cast<float>(rand()) / RAND_MAX) * lifetime * 0.3f;
            p.age = 0.0f;
            p.size = size + (static_cast<float>(rand()) / RAND_MAX) * size * 0.3f;
            p.startColor = startColor;
            p.endColor = endColor;
            
            ParticleSystem::getInstance().spawn(p);
        }
    }

    // Multi-frame burst emission
    if (burstActive) {
        burstElapsed += deltaTime;
        float t = (burstDuration > 0.0f) ? std::min(burstElapsed / burstDuration, 1.0f) : 1.0f;

        // Emit a portion this frame based on remaining particles and time left
        int remaining = burstCount - burstEmitted;
        if (remaining > 0) {
            // Aim to emit evenly across frames: particles per second = burstCount / burstDuration
            float targetPerSecond = (burstDuration > 0.0f) ? (static_cast<float>(burstCount) / burstDuration) : static_cast<float>(burstCount);
            int toEmit = static_cast<int>(std::ceil(targetPerSecond * deltaTime));
            toEmit = std::max(1, std::min(toEmit, remaining));

            // Spawn 'toEmit' particles now using same params
            float rotation = transform->getRotation() * 3.14159265f / 180.0f;
            float spawnX = transform->getX() + 
                          (offsetX * std::cos(rotation) - offsetY * std::sin(rotation));
            float spawnY = transform->getY() + 
                          (offsetX * std::sin(rotation) + offsetY * std::cos(rotation));

            for (int i = 0; i < toEmit; ++i) {
                float baseAngle = rotation + directionOffset * 3.14159265f / 180.0f;
                float spreadRad = spreadAngle * 3.14159265f / 180.0f;
                float randomSpread = ((static_cast<float>(rand()) / RAND_MAX) * spreadRad) - (spreadRad / 2.0f);
                float finalAngle = baseAngle + randomSpread;

                // Ease-out: early burst particles faster, later slower
                float ease = 1.0f - t;                // linear ease-out by time within burst
                float rnd = static_cast<float>(rand()) / RAND_MAX;
                float speed = speedMin + (speedMax - speedMin) * (0.5f * rnd + 0.5f * ease);

                Particle p;
                p.x = spawnX;
                p.y = spawnY;
                p.vx = std::cos(finalAngle) * speed;
                p.vy = std::sin(finalAngle) * speed;
                p.lifetime = lifetime + (static_cast<float>(rand()) / RAND_MAX) * lifetime * 0.3f;
                p.age = 0.0f;
                p.size = size + (static_cast<float>(rand()) / RAND_MAX) * size * 0.5f;
                p.startColor = startColor;
                p.endColor = endColor;

                ParticleSystem::getInstance().spawn(p);
            }
            burstEmitted += toEmit;
        }

        // End burst when duration elapsed or all emitted
        if (burstElapsed >= burstDuration || burstEmitted >= burstCount) {
            burstActive = false;
            burstElapsed = 0.0f;
            burstEmitted = 0;
        }
    }
}

void ParticleEmitterComponent::triggerBurst() {
    if (!transform) return;
    // Activate multi-frame burst; update() will emit across frames
    burstActive = true;
    burstElapsed = 0.0f;
    burstEmitted = 0;
}
