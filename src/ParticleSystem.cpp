#include "ParticleSystem.h"
#include "View.h"
#include <cmath>
#include <algorithm>

void ParticleSystem::spawn(const Particle& particle) {
    particles.push_back(particle);
}

void ParticleSystem::spawnBurst(float x, float y, int count, 
                                 float speedMin, float speedMax,
                                 float lifetime, float size,
                                 SDL_Color startColor, SDL_Color endColor) {
    const float PI = 3.14159265f;
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = x;
        p.y = y;
        
        // Random angle and speed
        float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * PI;
        float speed = speedMin + (static_cast<float>(rand()) / RAND_MAX) * (speedMax - speedMin);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        
        p.lifetime = lifetime + (static_cast<float>(rand()) / RAND_MAX) * lifetime * 0.3f; // ±30% variation
        p.age = 0.0f;
        p.size = size + (static_cast<float>(rand()) / RAND_MAX) * size * 0.5f; // ±50% variation
        p.startColor = startColor;
        p.endColor = endColor;
        
        particles.push_back(p);
    }
}

void ParticleSystem::update(float deltaTime) {
    // Update all particles and remove dead ones
    for (auto& p : particles) {
        p.age += deltaTime;
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
    }
    
    // Remove dead particles (erase-remove idiom)
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                      [](const Particle& p) { return !p.isAlive(); }),
        particles.end()
    );
}

void ParticleSystem::render(SDL_Renderer* renderer, View* view) {
    if (!renderer || !view) return;
    
    // Enable alpha blending for particle transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    for (const auto& p : particles) {
        // Create a world rect for the particle
        SDL_Rect worldRect = {
            static_cast<int>(p.x - p.size),
            static_cast<int>(p.y - p.size),
            static_cast<int>(p.size * 2),
            static_cast<int>(p.size * 2)
        };
        
        // Transform to screen coordinates
        SDL_Rect screenRect = view->worldToScreen(worldRect);
        
        // Get current color with fade
        SDL_Color color = p.getCurrentColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        
        // Draw particle as a filled rect
        SDL_RenderFillRect(renderer, &screenRect);
    }
    
    // Reset blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void ParticleSystem::clear() {
    particles.clear();
}
