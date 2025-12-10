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
    
    // Load sounds
    tinyxml2::XMLElement* soundsElement = root->FirstChildElement("sounds");
    if (soundsElement) {
        for (tinyxml2::XMLElement* soundElement = soundsElement->FirstChildElement("sound");
             soundElement != nullptr;
             soundElement = soundElement->NextSiblingElement("sound")) {
            
            const char* id = soundElement->Attribute("id");
            const char* path = soundElement->Attribute("path");
            
            if (id && path) {
                Mix_Chunk* sound = Mix_LoadWAV(path);
                if (sound) {
                    sounds[id] = sound;
                    std::cout << "Loaded sound: " << id << " from " << path << std::endl;
                } else {
                    std::cerr << "Failed to load sound " << path << ": " << Mix_GetError() << std::endl;
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

bool AssetManager::getTextureDimensions(const std::string& id, int& width, int& height) {
    SDL_Texture* texture = getTexture(id);
    if (texture) {
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return true;
    }
    return false;
}

Mix_Chunk* AssetManager::getSound(const std::string& id) {
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::playSound(const std::string& id) {
    Mix_Chunk* sound = getSound(id);
    if (sound) {
        Mix_PlayChannel(-1, sound, 0);
    }
}

int AssetManager::playSoundLoop(const std::string& id) {
    Mix_Chunk* sound = getSound(id);
    if (sound) {
        return Mix_PlayChannel(-1, sound, -1);  // -1 loops indefinitely
    }
    return -1;
}

void AssetManager::stopSound(int channel) {
    if (channel >= 0) {
        Mix_HaltChannel(channel);
    }
}

void AssetManager::clean() {
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    
    for (auto& pair : sounds) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds.clear();
    
    Mix_CloseAudio();
}

void AssetManager::setHighScore(int score) {
    if (score > highScore) {
        highScore = score;
    }
}

bool AssetManager::loadHighScoreFromXML(const std::string& filepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML for high score: " << filepath << std::endl;
        return false;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("assets");
    if (!root) {
        return false;
    }
    
    tinyxml2::XMLElement* gameElement = root->FirstChildElement("game");
    if (gameElement) {
        const char* scoreStr = gameElement->Attribute("highScore");
        if (scoreStr) {
            highScore = std::stoi(scoreStr);
            std::cout << "Loaded high score: " << highScore << std::endl;
            return true;
        }
    }
    return false;
}

bool AssetManager::saveHighScoreToXML(const std::string& filepath) const {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML for saving high score: " << filepath << std::endl;
        return false;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("assets");
    if (!root) {
        return false;
    }
    
    // Find or create game element
    tinyxml2::XMLElement* gameElement = root->FirstChildElement("game");
    if (!gameElement) {
        gameElement = doc.NewElement("game");
        root->InsertEndChild(gameElement);
    }
    
    gameElement->SetAttribute("highScore", highScore);
    
    if (doc.SaveFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to save high score to XML: " << filepath << std::endl;
        return false;
    }
    
    std::cout << "Saved high score: " << highScore << " to " << filepath << std::endl;
    return true;
}