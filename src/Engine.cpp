#include "Engine.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "PlayerControlComponent.h"
#include "CameraFollowComponent.h"
#include "View.h"
#include <tinyxml2.h>
#include <iostream>

int Engine::targetFPS = 60;
float Engine::deltaTime = 0.0f;

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::init(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    running = true;
    lastFrameTime = SDL_GetTicks();
    
    // Load game objects from XML
    loadGameObjectsFromXML("assets/config.xml");
    
    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

void Engine::loadGameObjectsFromXML(const std::string& filepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load game objects XML: " << filepath << std::endl;
        return;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("assets");
    if (!root) return;
    
    tinyxml2::XMLElement* gameObjectsElement = root->FirstChildElement("gameObjects");
    if (!gameObjectsElement) return;
    
    for (tinyxml2::XMLElement* objElement = gameObjectsElement->FirstChildElement("object");
         objElement != nullptr;
         objElement = objElement->NextSiblingElement("object")) {
        
        const char* type = objElement->Attribute("type");
        if (!type) continue;
        
        auto gameObj = std::make_unique<GameObject>();
        
        // Add Transform component
        auto* transform = gameObj->addComponent<TransformComponent>();
        tinyxml2::XMLElement* posElement = objElement->FirstChildElement("position");
        if (posElement) {
            float x = posElement->FloatAttribute("x", 0.0f);
            float y = posElement->FloatAttribute("y", 0.0f);
            transform->setPosition(x, y);
        }
        
        // Add Sprite component
        auto* sprite = gameObj->addComponent<SpriteComponent>();
        tinyxml2::XMLElement* spriteElement = objElement->FirstChildElement("sprite");
        if (spriteElement) {
            const char* texture = spriteElement->Attribute("texture");
            if (texture) sprite->setTexture(texture);
            
            float width = spriteElement->FloatAttribute("width", 50.0f);
            float height = spriteElement->FloatAttribute("height", 50.0f);
            sprite->setSize(width, height);
        }
        
        // Add type-specific components
        if (std::string(type) == "player") {
            gameObj->addComponent<InputComponent>();
            gameObj->addComponent<PlayerControlComponent>();
            gameObj->addComponent<CameraFollowComponent>();
        }
        
        gameObjects.push_back(std::move(gameObj));
    }
    
    std::cout << "Loaded " << gameObjects.size() << " game objects from XML" << std::endl;
}

void Engine::run() {
    const int frameDelay = 1000 / targetFPS;
    
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        
        handleEvents();
        update();
        render();
        
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        deltaTime = frameTime / 1000.0f;
        
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
            deltaTime = frameDelay / 1000.0f;
        }
    }
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
    }
}

void Engine::update() {
    // Update all game objects
    for (auto& obj : gameObjects) {
        obj->update(deltaTime);
    }
}

void Engine::render() {
    // Clear screen with dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    
    // Render all game objects
    for (auto& obj : gameObjects) {
        obj->render();
    }
    
    SDL_RenderPresent(renderer);
}

void Engine::quit() {
    running = false;
}

void Engine::clean() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Engine cleaned up" << std::endl;
}

void Engine::setTargetFPS(int fps) {
    targetFPS = fps;
}
