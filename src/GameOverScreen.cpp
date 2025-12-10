#include "GameOverScreen.h"
#include <SDL_ttf.h>
#include <iostream>

GameOverScreen::GameOverScreen(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, int windowWidth, int windowHeight)
    : renderer(renderer), fontPath(fontPath), fontSize(fontSize), windowWidth(windowWidth), windowHeight(windowHeight) {
}

GameOverScreen::~GameOverScreen() {
    cleanupTextureData(gameOverText);
    cleanupTextureData(scoreText);
    cleanupTextureData(highScoreText);
}

void GameOverScreen::cleanupTextureData(TextureData& data) {
    if (data.texture) {
        SDL_DestroyTexture(data.texture);
        data.texture = nullptr;
    }
}

GameOverScreen::TextureData GameOverScreen::createTextTexture(const std::string& text, SDL_Color color) {
    TextureData data;
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) return data;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        TTF_CloseFont(font);
        return data;
    }
    
    data.texture = SDL_CreateTextureFromSurface(renderer, surface);
    data.destRect = { 0, 0, surface->w, surface->h };
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    
    return data;
}

void GameOverScreen::updateTextures() {
    cleanupTextureData(gameOverText);
    cleanupTextureData(scoreText);
    cleanupTextureData(highScoreText);
    
    SDL_Color whiteColor = {255, 255, 255, 255};
    SDL_Color yellowColor = {255, 255, 0, 255};
    SDL_Color greenColor = {0, 255, 0, 255};
    
    gameOverText = createTextTexture("GAME OVER", whiteColor);
    scoreText = createTextTexture("Score: " + std::to_string(score), yellowColor);
    highScoreText = createTextTexture("High Score: " + std::to_string(highScore), greenColor);
}

void GameOverScreen::render() {
    // Semi-transparent black background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullScreen = { 0, 0, windowWidth, windowHeight };
    SDL_RenderFillRect(renderer, &fullScreen);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    // Center texts on screen
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    
    // Game Over text (top)
    if (gameOverText.texture) {
        gameOverText.destRect.x = centerX - gameOverText.destRect.w / 2;
        gameOverText.destRect.y = centerY - 150;
        SDL_RenderCopy(renderer, gameOverText.texture, nullptr, &gameOverText.destRect);
    }
    
    // Score text (middle)
    if (scoreText.texture) {
        scoreText.destRect.x = centerX - scoreText.destRect.w / 2;
        scoreText.destRect.y = centerY - 30;
        SDL_RenderCopy(renderer, scoreText.texture, nullptr, &scoreText.destRect);
    }
    
    // High score text (bottom)
    if (highScoreText.texture) {
        highScoreText.destRect.x = centerX - highScoreText.destRect.w / 2;
        highScoreText.destRect.y = centerY + 60;
        SDL_RenderCopy(renderer, highScoreText.texture, nullptr, &highScoreText.destRect);
    }
}
