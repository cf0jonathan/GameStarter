#pragma once
#include <SDL.h>
#include <string>

class GameOverScreen {
public:
    GameOverScreen(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, int windowWidth, int windowHeight);
    ~GameOverScreen();
    
    void setScore(int score) { this->score = score; updateTextures(); }
    void setHighScore(int highScore) { this->highScore = highScore; updateTextures(); }
    void render();
    
private:
    SDL_Renderer* renderer;
    std::string fontPath;
    int fontSize;
    int windowWidth, windowHeight;
    int score = 0;
    int highScore = 0;
    
    struct TextureData {
        SDL_Texture* texture = nullptr;
        SDL_Rect destRect{};
    };
    
    TextureData gameOverText;
    TextureData scoreText;
    TextureData highScoreText;
    
    void updateTextures();
    TextureData createTextTexture(const std::string& text, SDL_Color color);
    void cleanupTextureData(TextureData& data);
};
