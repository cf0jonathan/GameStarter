#include "Engine.h"
#include "AssetManager.h"
#include <iostream>

int main() {
    Engine& engine = Engine::getInstance();
    
    if (!engine.init("Bare Minimum Game", 800, 600)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    // Set target frame rate to 60 FPS
    Engine::setTargetFPS(60);
    
    // Load assets from XML
    AssetManager::getInstance().loadFromXML("assets/config.xml");
    
    std::cout << "Starting game loop..." << std::endl;
    engine.run();
    
    // Cleanup
    AssetManager::getInstance().clean();
    engine.clean();
    
    return 0;
}