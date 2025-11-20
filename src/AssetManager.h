#pragma once
#include <string>
#include <map>
#include <SDL2/SDL.h>

class AssetManager {
public:
    static AssetManager& getInstance();
    
    bool loadFromXML(const std::string& filepath);
    SDL_Texture* getTexture(const std::string& id);
    bool getTextureDimensions(const std::string& id, int& width, int& height);
    
    void clean();
    
private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    
    std::map<std::string, SDL_Texture*> textures;
};
