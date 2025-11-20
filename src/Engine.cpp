#include "Engine.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "PlayerControlComponent.h"
#include "CameraFollowComponent.h"
#include "RotateToMouseComponent.h"
#include "MoveToMouseComponent.h"
#include "BackgroundComponent.h"
#include "View.h"
#include <tinyxml2.h>
#include <iostream>

int Engine::targetFPS = 60;
float Engine::deltaTime = 0.0f;
int Engine::mouseX = 0;
int Engine::mouseY = 0;
int Engine::logicFPS = 60;
float Engine::fixedDeltaTime = 1.0f / 60.0f;

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
    
    windowWidth = width;
    windowHeight = height;
    
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
        
        // Handle background type separately (no transform, no sprite)
        if (std::string(type) == "background") {
            auto* background = gameObj->addComponent<BackgroundComponent>();
            tinyxml2::XMLElement* bgElement = objElement->FirstChildElement("background");
            if (bgElement) {
                const char* texture = bgElement->Attribute("texture");
                if (texture) background->setTexture(texture);
                
                float tileWidth = bgElement->FloatAttribute("tileWidth", 800.0f);
                float tileHeight = bgElement->FloatAttribute("tileHeight", 600.0f);
                background->setTileSize(tileWidth, tileHeight);
                
                float scrollX = bgElement->FloatAttribute("scrollX", 0.0f);
                float scrollY = bgElement->FloatAttribute("scrollY", 0.0f);
                background->setScrollSpeed(scrollX, scrollY);
            }
            gameObjects.push_back(std::move(gameObj));
            continue;
        }
        
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
            
            // Support either explicit width+height or single dimension with aspect preservation
            bool hasWidth = spriteElement->Attribute("width") != nullptr;
            bool hasHeight = spriteElement->Attribute("height") != nullptr;
            
            if (hasWidth && hasHeight) {
                // Both specified: use explicit dimensions
                float width = spriteElement->FloatAttribute("width");
                float height = spriteElement->FloatAttribute("height");
                sprite->setSize(width, height);
            } else if (hasWidth) {
                // Only width: preserve aspect ratio from width
                float width = spriteElement->FloatAttribute("width");
                sprite->setSizePreserveAspect(width, true);
            } else if (hasHeight) {
                // Only height: preserve aspect ratio from height
                float height = spriteElement->FloatAttribute("height");
                sprite->setSizePreserveAspect(height, false);
            } else {
                // Neither specified: use default 50x50
                sprite->setSize(50.0f, 50.0f);
            }
        }
        
        // Add type-specific components
        if (std::string(type) == "player") {
            gameObj->addComponent<InputComponent>();
            gameObj->addComponent<PlayerControlComponent>();
            gameObj->addComponent<RotateToMouseComponent>();
            
            // Add MoveToMouseComponent with configurable speed and arrival radius
            auto* moveToMouse = gameObj->addComponent<MoveToMouseComponent>();
            tinyxml2::XMLElement* moveElement = objElement->FirstChildElement("moveToMouse");
            if (moveElement) {
                float speed = moveElement->FloatAttribute("speed", 200.0f);
                float arrivalRadius = moveElement->FloatAttribute("arrivalRadius", 5.0f);
                moveToMouse->setMoveSpeed(speed);
                moveToMouse->setArrivalRadius(arrivalRadius);
            }
            
            gameObj->addComponent<CameraFollowComponent>();
        }
        
        gameObjects.push_back(std::move(gameObj));
    }
    
    std::cout << "Loaded " << gameObjects.size() << " game objects from XML" << std::endl;
}

void Engine::run() {
    const int frameDelay = 1000 / targetFPS; // render cap
    Uint32 previousTicks = SDL_GetTicks();
    float accumulator = 0.0f;
    
    while (running) {
        Uint32 currentTicks = SDL_GetTicks();
        float frameTime = (currentTicks - previousTicks) / 1000.0f; // seconds
        previousTicks = currentTicks;
        
        // Clamp very large frame time to avoid spiral of death
        if (frameTime > 0.25f) frameTime = 0.25f;
        accumulator += frameTime;
        
        handleEvents();
        
        // Fixed timestep updates
        while (accumulator >= fixedDeltaTime) {
            deltaTime = fixedDeltaTime; // expose for components querying getDeltaTime()
            update();
            accumulator -= fixedDeltaTime;
        }
        
        render();
        
        // Frame limiting for rendering only
        Uint32 afterRenderTicks = SDL_GetTicks();
        Uint32 loopFrameTimeMs = afterRenderTicks - currentTicks;
        if (frameDelay > loopFrameTimeMs) {
            SDL_Delay(frameDelay - loopFrameTimeMs);
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
        if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        }
    }
}

void Engine::update() {
    // Use fixedDeltaTime to ensure movement independent of render FPS
    for (auto& obj : gameObjects) {
        obj->update(fixedDeltaTime);
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

void Engine::setLogicFPS(int fps) {
    if (fps <= 0) return;
    logicFPS = fps;
    fixedDeltaTime = 1.0f / static_cast<float>(logicFPS);
}
