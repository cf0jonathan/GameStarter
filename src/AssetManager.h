#pragma once
#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class AssetManager {
public:
    static AssetManager& getInstance();
    
    bool loadFromXML(const std::string& filepath);
    SDL_Texture* getTexture(const std::string& id);
    bool getTextureDimensions(const std::string& id, int& width, int& height);
    
    // Audio support
    Mix_Chunk* getSound(const std::string& id);
    void playSound(const std::string& id);
    int playSoundLoop(const std::string& id);  // Returns channel ID
    void stopSound(int channel);
    void stopAllSounds();                      // Stop all currently playing sounds
    void setVolume(const std::string& id, int volume); // Set volume (0-128) for a specific sound
    
    // High score persistence
    int getHighScore() const { return highScore; }
    void setHighScore(int score);
    bool loadHighScoreFromXML(const std::string& filepath);
    bool saveHighScoreToXML(const std::string& filepath) const;
    
    void clean();
    
private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, Mix_Chunk*> sounds;
    int highScore = 0;
};
