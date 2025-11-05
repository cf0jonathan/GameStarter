#pragma once
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <tinyxml2.h>
#include "GameObject.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "ControllerComponent.h"
#include "SlideComponent.h"
#include "RotateComponent.h"

class ComponentFactory {
public:
    // Type alias for component creation functions
    using ComponentCreator = std::function<void(GameObject*, tinyxml2::XMLElement*)>;
    
    ComponentFactory() {
        // Register all component creators
        componentCreators["BodyComponent"] = [](GameObject* obj, tinyxml2::XMLElement* elem) {
            double x = elem->DoubleAttribute("x", 0.0);
            double y = elem->DoubleAttribute("y", 0.0);
            double vx = elem->DoubleAttribute("vx", 0.0);
            double vy = elem->DoubleAttribute("vy", 0.0);
            double angle = elem->DoubleAttribute("angle", 0.0);
            obj->add<BodyComponent>(x, y, vx, vy, angle);
        };
        
        componentCreators["SpriteComponent"] = [](GameObject* obj, tinyxml2::XMLElement* elem) {
            const char* textureKey = elem->Attribute("textureKey");
            if (textureKey) {
                obj->add<SpriteComponent>(std::string(textureKey));
            }
        };
        
        componentCreators["ControllerComponent"] = [](GameObject* obj, tinyxml2::XMLElement* elem) {
            double speed = elem->DoubleAttribute("speed", 5.0);
            double rotationSpeed = elem->DoubleAttribute("rotationSpeed", 3.0);
            obj->add<ControllerComponent>(speed, rotationSpeed);
        };
        
        componentCreators["SlideComponent"] = [](GameObject* obj, tinyxml2::XMLElement* elem) {
            double speed = elem->DoubleAttribute("speed", 3.0);
            double leftBound = elem->DoubleAttribute("leftBound", 0.0);
            double rightBound = elem->DoubleAttribute("rightBound", 800.0);
            obj->add<SlideComponent>(speed, leftBound, rightBound);
        };
        
        componentCreators["RotateComponent"] = [](GameObject* obj, tinyxml2::XMLElement* elem) {
            double rotationSpeed = elem->DoubleAttribute("rotationSpeed", 1.0);
            obj->add<RotateComponent>(rotationSpeed);
        };
    }
    
    // Load level from XML file
    std::vector<std::unique_ptr<GameObject>> loadLevel(const std::string& filePath) {
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
            SDL_Log("Failed to load XML file: %s", filePath.c_str());
            return gameObjects;
        }
        
        tinyxml2::XMLElement* level = doc.FirstChildElement("Level");
        if (!level) {
            SDL_Log("No Level element found in XML");
            return gameObjects;
        }
        
        // Process each GameObject
        for (tinyxml2::XMLElement* gameObjectElem = level->FirstChildElement("GameObject");
             gameObjectElem != nullptr;
             gameObjectElem = gameObjectElem->NextSiblingElement("GameObject")) {
            
            auto gameObject = std::make_unique<GameObject>();
            
            // Process each component in this GameObject
            for (tinyxml2::XMLElement* componentElem = gameObjectElem->FirstChildElement();
                 componentElem != nullptr;
                 componentElem = componentElem->NextSiblingElement()) {
                
                std::string componentName = componentElem->Name();
                
                // Find and call the appropriate creator function
                auto it = componentCreators.find(componentName);
                if (it != componentCreators.end()) {
                    it->second(gameObject.get(), componentElem);
                    SDL_Log("Added %s to GameObject", componentName.c_str());
                } else {
                    SDL_Log("Unknown component type: %s", componentName.c_str());
                }
            }
            
            gameObjects.push_back(std::move(gameObject));
        }
        
        SDL_Log("Loaded %zu GameObjects from %s", gameObjects.size(), filePath.c_str());
        return gameObjects;
    }
    
private:
    std::unordered_map<std::string, ComponentCreator> componentCreators;
};