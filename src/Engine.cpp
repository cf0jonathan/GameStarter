#include "Engine.h"
#include "Object.h"
#include <tinyxml2.h>
#include <iostream>
#include <fstream>


Engine::Engine() {
    std::cout << "[Engine] Engine created with empty level\n";
}

Engine::Engine(const std::string& levelPath) {
    std::cout << "[Engine] Engine created, loading level: " << levelPath << "\n";
    loadLevel(levelPath);
}

void Engine::loadLevel(const std::string& levelPath) {
    std::cout << "[Engine] Loading level from: " << levelPath << "\n";
    
    objects.clear();
    
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLError result = doc.LoadFile(levelPath.c_str());
    
    if (result != tinyxml2::XML_SUCCESS) {
        std::cout << "[Engine] ERROR: Failed to load XML file: " << levelPath << "\n";
        std::cout << "[Engine] XMLError code: " << result << "\n";
        return;
    }
    
    const auto* levelElement = doc.FirstChildElement("Level");
    if (!levelElement) {
        std::cout << "[Engine] ERROR: No <Level> root element found\n";
        return;
    }
    
    std::cout << "[Engine] Found <Level> element, parsing objects...\n";
    
    int objectCount = 0;
    for (const auto* objectElement = levelElement->FirstChildElement("Object");
         objectElement != nullptr;
         objectElement = objectElement->NextSiblingElement("Object")) {

        const char* typeAttr = objectElement->Attribute("type");
        if (!typeAttr) {
            std::cout << "[Engine] WARNING: <Object> element missing 'type' attribute, skipping\n";
            continue;
        }
        
        std::string objectType = typeAttr;
        std::cout << "[Engine] Processing object type: " << objectType << "\n";
        
        auto newObject = objectLibrary.create(objectType, objectElement);
        
        if (newObject) {
            objects.push_back(std::move(newObject));
            objectCount++;
        } else {
            std::cout << "[Engine] WARNING: Failed to create object of type: " << objectType << "\n";
        }
    }

    std::cout << "[Engine] Level loaded successfully. Created " << objectCount << " objects.\n";
}


void Engine::update() {
    for (auto& object : objects) {
        if (object) {
            object->update();
        }
    }
}


void Engine::draw() {
    std::cout << "[Engine] Drawing " << objects.size() << " objects:\n";
    for (auto& object : objects) {
        if (object) {
            object->draw();
        }
    }
}