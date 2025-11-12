#include "AssetManager.h"
#include "Engine.h"
#include <SDL2/SDL_image.h>
#include <tinyxml2.h>
#include <iostream>

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

bool AssetManager::loadFromXML(const std::string& filepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML: " << filepath << std::endl;
        return false;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("assets");
    if (!root) {
        std::cerr << "No assets root element found" << std::endl;
        return false;
    }
    
    tinyxml2::XMLElement* texturesElement = root->FirstChildElement("textures");
    if (texturesElement) {
        for (tinyxml2::XMLElement* textureElement = texturesElement->FirstChildElement("texture");
             textureElement != nullptr;
             textureElement = textureElement->NextSiblingElement("texture")) {
            
            const char* id = textureElement->Attribute("id");
            const char* path = textureElement->Attribute("path");
            
            if (id && path) {
                SDL_Surface* surface = IMG_Load(path);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(
                        Engine::getInstance().getRenderer(), surface);
                    SDL_FreeSurface(surface);
                    
                    if (texture) {
                        textures[id] = texture;
                        std::cout << "Loaded texture: " << id << " from " << path << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << "Asset loading complete" << std::endl;
    return true;
}

SDL_Texture* AssetManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::clean() {
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
}
