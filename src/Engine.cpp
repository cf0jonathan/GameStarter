#include "Engine.h"
#include "ProjectileComponent.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"

// Definition of static members
bool Engine::isRunning = false;
SDL_Window* Engine::window = nullptr;
SDL_Renderer* Engine::renderer = nullptr;
std::vector<std::unique_ptr<GameObject>> Engine::gameObjects;
int Engine::width = 0;
int Engine::height = 0;

// Helper function for SpriteComponent to avoid circular includes
SDL_Renderer* getGameRenderer() {
    return Engine::getRenderer();
}

// Create and add a projectile
void Engine::spawnProjectile(double x, double y, double angle) {
    SDL_Log("Spawning projectile at (%.1f, %.1f) angle %.1f", x, y, angle);
    SDL_Log("Current object count: %zu", gameObjects.size());
    
    auto projectile = std::make_unique<GameObject>();
    projectile->add<BodyComponent>(x, y);
    projectile->add<SpriteComponent>("acorn");
    projectile->add<ProjectileComponent>(10.0, angle, width, height);  // Pass screen dimensions
    gameObjects.push_back(std::move(projectile));
    
    SDL_Log("New object count: %zu", gameObjects.size());
}

// Remove projectiles that are off-screen
void Engine::cleanupProjectiles() {
    // Use a safer approach - iterate with explicit index
    size_t initialSize = gameObjects.size();
    for (size_t i = 0; i < gameObjects.size(); ) {
        auto projectile = gameObjects[i]->get<ProjectileComponent>();
        if (projectile && projectile->shouldDestroy()) {
            gameObjects.erase(gameObjects.begin() + i);
            // Don't increment i since we removed an element
        } else {
            ++i;
        }
    }
    
    Log cleanup activity
    if (gameObjects.size() != initialSize) {
        SDL_Log("Cleaned up %zu projectiles, %zu objects remaining", 
                initialSize - gameObjects.size(), gameObjects.size());
    }
}