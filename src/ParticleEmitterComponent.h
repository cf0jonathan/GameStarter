#pragma once
#include "Component.h"
#include <SDL.h>

class TransformComponent;

enum class EmitterType {
    Continuous,  // Steady stream (engine trail)
    Burst        // One-time explosion
};

class ParticleEmitterComponent : public Component {
public:
    ParticleEmitterComponent() = default;
    ~ParticleEmitterComponent() override = default;
    
    void init() override;
    void update(float deltaTime) override;
    
    // Configuration
    void setEmitterType(EmitterType type) { emitterType = type; }
    void setSpawnRate(float rate) { spawnRate = rate; }  // particles per second (continuous only)
    void setBurstCount(int count) { burstCount = count; }  // particles per burst
    void setLifetime(float time) { lifetime = time; }
    void setSpeedMin(float speed) { speedMin = speed; }
    void setSpeedMax(float speed) { speedMax = speed; }
    void setSize(float s) { size = s; }
    void setStartColor(SDL_Color color) { startColor = color; }
    void setEndColor(SDL_Color color) { endColor = color; }
    void setSpreadAngle(float angle) { spreadAngle = angle; }  // cone angle in degrees (0 = all directions)
    void setDirectionOffset(float offset) { directionOffset = offset; }  // angle offset from facing direction
    void setPositionOffset(float x, float y) { offsetX = x; offsetY = y; }  // spawn offset from transform
    void setActive(bool active) { isActive = active; }
    
    // Trigger a burst emission (for explosions)
    void triggerBurst();
    
    // Emission control
    void setRequireMouseButton(bool require) { requireMouseButton = require; }
    void setBurstDuration(float dur) { burstDuration = dur; }
    
private:
    TransformComponent* transform = nullptr;
    class InputComponent* input = nullptr;
    
    EmitterType emitterType = EmitterType::Continuous;
    bool isActive = true;
    bool requireMouseButton = false;  // Only emit when left mouse is held
    
    // Spawn parameters
    float spawnRate = 50.0f;           // particles/sec for continuous
    int burstCount = 30;               // total particles per burst
    float lifetime = 0.5f;             // particle lifetime in seconds
    float speedMin = 50.0f;            // minimum particle speed
    float speedMax = 150.0f;           // maximum particle speed
    float size = 3.0f;                 // particle size (radius)
    float spreadAngle = 360.0f;        // spread cone in degrees (360 = full circle)
    float directionOffset = 180.0f;    // angle offset from facing direction (180 = behind)
    float offsetX = 0.0f;              // spawn position offset
    float offsetY = 0.0f;
    
    SDL_Color startColor = {255, 128, 0, 255};  // orange
    SDL_Color endColor = {255, 0, 0, 0};        // red fading to transparent
    
    // State for continuous emission
    float accumulator = 0.0f;

    // State for multi-frame burst emission
    bool burstActive = false;
    float burstElapsed = 0.0f;
    float burstDuration = 0.15f;   // seconds to spread the burst over
    int burstEmitted = 0;          // how many particles already emitted
};
