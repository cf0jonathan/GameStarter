#include "Engine.h"
#include "GameObject.h"
#include "ComponentFactory.h"

int main(int argc, char* argv[]) {
    // Initialize the engine
    if (!Engine::init("Component-Based Game Engine", 800, 600)) {
        return -1;
    }

    SDL_Renderer* renderer = Engine::getRenderer();

    // Load textures
    Textures::load("squirrel", "assets/squirrel.png", renderer);
    Textures::load("acorn", "assets/acorn.png", renderer);

    // Create factory and load level from XML
    ComponentFactory factory;
    auto gameObjects = factory.loadLevel("assets/level.xml");
    
    // Find the player object and set up projectile spawning
    for (auto& obj : gameObjects) {
        auto controller = obj->get<ControllerComponent>();
        if (controller) {
            // Set up the projectile spawn callback
            controller->setProjectileSpawnCallback([](double x, double y, double angle) {
                Engine::spawnProjectile(x, y, angle);
            });
        }
        Engine::addGameObject(std::move(obj));
    }

    // Run the game loop
    Engine::run();

    return 0;
}