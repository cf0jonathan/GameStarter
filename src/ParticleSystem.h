#pragma once
#include <SDL.h>
#include <vector>

// Lightweight particle struct (not a GameObject)
struct Particle {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;  // velocity x
    float vy = 0.0f;  // velocity y
    float lifetime = 1.0f;      // total lifetime in seconds
    float age = 0.0f;           // current age in seconds
    float size = 2.0f;          // radius in pixels
    SDL_Color startColor = {255, 255, 255, 255};
    SDL_Color endColor = {255, 255, 255, 0};  // fade to transparent
    
    bool isAlive() const { return age < lifetime; }
    float getAlpha() const {
        float t = age / lifetime;
        return startColor.a * (1.0f - t) + endColor.a * t;
    }
    SDL_Color getCurrentColor() const {
        float t = age / lifetime;
        SDL_Color color;
        color.r = static_cast<Uint8>(startColor.r * (1.0f - t) + endColor.r * t);
        color.g = static_cast<Uint8>(startColor.g * (1.0f - t) + endColor.g * t);
        color.b = static_cast<Uint8>(startColor.b * (1.0f - t) + endColor.b * t);
        color.a = static_cast<Uint8>(getAlpha());
        return color;
    }
};

// Singleton particle system manager
class ParticleSystem {
public:
    static ParticleSystem& getInstance() {
        static ParticleSystem instance;
        return instance;
    }
    
    // Prevent copy/assignment
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;
    
    // Add a single particle
    void spawn(const Particle& particle);
    
    // Spawn multiple particles in a burst (for explosions)
    void spawnBurst(float x, float y, int count, float speedMin, float speedMax,
                    float lifetime, float size,
                    SDL_Color startColor, SDL_Color endColor);
    
    // Update all particles (remove dead ones)
    void update(float deltaTime);
    
    // Render all particles
    void render(SDL_Renderer* renderer, class View* view);
    
    // Clear all particles
    void clear();
    
    // Get particle count for debugging
    size_t getParticleCount() const { return particles.size(); }
    
private:
    ParticleSystem() = default;
    std::vector<Particle> particles;
};
