#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>  // For std::unique_ptr
#include <algorithm>  // For std::remove_if
#include "Input.h"
#include "GameObject.h"

// Forward declarations to avoid circular includes
class ProjectileComponent;

class Engine {
public:
    static int width;
    static int height;

    // Initialize the Engine (static)
    static bool init(const char* title, int width, int height) {
        Engine::width = width;
        Engine::height = height;
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
            return false;
        }

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        if (!window) {
            SDL_Log("Failed to create window: %s", SDL_GetError());
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            SDL_Log("Failed to create renderer: %s", SDL_GetError());
            return false;
        }

        isRunning = true;
        return true;
    }

    // Handle events (static)
    static void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            Input::processEvent(event);  // Call static Input method
        }
    }

    // Update all game objects (static)
    static void update() {
        // Debug: Log object count periodically
        static int frameCount = 0;
        frameCount++;
        if (frameCount % 60 == 0) {  // Every second at 60fps
            SDL_Log("Frame %d: %zu objects in scene", frameCount, gameObjects.size());
        }
        
        for (size_t i = 0; i < gameObjects.size(); ++i) {
            if (gameObjects[i]) {
                gameObjects[i]->update();  // Update each GameObject
            } else {
                SDL_Log("WARNING: Null GameObject at index %zu", i);
            }
        }
        cleanupProjectiles();  // Remove off-screen projectiles
    }

    // Render all game objects (static)
    static void render() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (size_t i = 0; i < gameObjects.size(); ++i) {
            if (gameObjects[i]) {
                gameObjects[i]->draw();  // Draw each GameObject
            } else {
                SDL_Log("WARNING: Null GameObject during render at index %zu", i);
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up SDL resources (static)
    static void clean() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }

    // Check if the engine is still running (static)
    static bool running() {
        return isRunning;
    }

    // Add a GameObject to the engine (static)
    static void addGameObject(std::unique_ptr<GameObject> gameObject) {
        gameObjects.push_back(std::move(gameObject));  // Add the game object to the list
    }

    // Create and add a projectile
    static void spawnProjectile(double x, double y, double angle);

    // Remove projectiles that are off-screen
    static void cleanupProjectiles();

    // Run the engine (static)
    static void run() {
        while (isRunning) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }

        clean();
    }

    static SDL_Renderer* getRenderer() {
        return renderer;
    }

private:
    static bool isRunning;                               // Engine running state (static)
    static SDL_Window* window;                           // SDL window (static)
    static SDL_Renderer* renderer;                       // SDL renderer (static)
    static std::vector<std::unique_ptr<GameObject>> gameObjects;  // Track game objects
};
