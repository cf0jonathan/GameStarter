#include "Engine.h"
#include "AssetManager.h"
#include "Settings.h"
#include <iostream>

int main() {

    Settings settings; // defaults populated
    loadSettingsFromXML("assets/config.xml", settings); // ignore failure, defaults remain

    Engine& engine = Engine::getInstance();
    
    if (!engine.init("Bare Minimum Game", settings.width, settings.height)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    Engine::setTargetFPS(settings.renderFPS);
    Engine::setLogicFPS(settings.logicFPS);
    
    // Load assets first (textures need to be loaded before game objects)
    AssetManager::getInstance().loadFromXML("assets/config.xml");
    
    // Load game objects (now textures are available for aspect ratio queries)
    engine.loadGameObjectsFromXML("assets/config.xml");
    
    std::cout << "Starting game loop..." << std::endl;
    engine.run();
    
    // Cleanup
    AssetManager::getInstance().clean();
    engine.clean();
    
    return 0;
}