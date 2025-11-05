#define SDL_MAIN_HANDLED
#include "Engine.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "BodyComponent.h"
#include "SlideComponent.h"

int main(int argc, char* argv[]) {
    // Initialize the engine
    if (!Engine::init("SDL2 Game Engine", 800, 600)) {
        return -1;
    }

    SDL_Renderer* renderer = Engine::getRenderer();  // Assuming Engine provides a getRenderer method

    // Load textures into the texture manager
    Textures::load("hero", "squirrel.png", renderer);
    Textures::load("enemy", "squirrel.png", renderer);

    // Create GameObject 1 and add SpriteComponent and BodyComponent
    auto gameObject1 = std::make_unique<GameObject>();
    gameObject1->add<SpriteComponent>("hero");  // Use texture key "hero"
    gameObject1->add<BodyComponent>(100, 200);  // BodyComponent with position (100, 200)

    // Create GameObject 2 and add SpriteComponent and BodyComponent
    auto gameObject2 = std::make_unique<GameObject>();
    gameObject2->add<SpriteComponent>("enemy");  // Use texture key "enemy"
    gameObject2->add<BodyComponent>(300, 400);  // BodyComponent with position (300, 400)
    gameObject2->add<SlideComponent>(10,100,200);  // BodyComponent with position (300, 400)

    // Add game objects to the engine
    Engine::addGameObject(std::move(gameObject1));
    Engine::addGameObject(std::move(gameObject2));

    // Run the engine, which will handle updating and drawing the game objects
    Engine::run();

    return 0;
}
