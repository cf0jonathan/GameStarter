#include "BackgroundComponent.h"
#include "Engine.h"
#include "View.h"
#include "AssetManager.h"
#include <SDL2/SDL.h>
#include <cmath>

void BackgroundComponent::render() {
    if (textureId.empty()) return;
    
    SDL_Texture* texture = AssetManager::getInstance().getTexture(textureId);
    if (!texture) return;
    
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    
    // Get view center for parallax scrolling
    float viewX, viewY;
    View::getInstance().getCenter(viewX, viewY);
    
    // Calculate offset based on scroll speed (parallax effect)
    // Negate to scroll in the opposite direction of camera movement
    offsetX = -viewX * scrollSpeedX;
    offsetY = -viewY * scrollSpeedY;
    
    // Get window dimensions
    int windowWidth = Engine::getInstance().getWindowWidth();
    int windowHeight = Engine::getInstance().getWindowHeight();
    
    // Calculate how many tiles we need to cover the screen
    int tilesX = static_cast<int>(std::ceil(windowWidth / tileWidth)) + 2;
    int tilesY = static_cast<int>(std::ceil(windowHeight / tileHeight)) + 2;
    
    // Calculate starting position (wrapped to tile size)
    float startX = std::fmod(offsetX, tileWidth);
    float startY = std::fmod(offsetY, tileHeight);
    
    // Adjust to ensure we start off-screen
    if (startX > 0) startX -= tileWidth;
    if (startY > 0) startY -= tileHeight;
    
    // Draw tiled background
    for (int y = 0; y < tilesY; ++y) {
        for (int x = 0; x < tilesX; ++x) {
            SDL_Rect destRect = {
                static_cast<int>(startX + x * tileWidth),
                static_cast<int>(startY + y * tileHeight),
                static_cast<int>(tileWidth),
                static_cast<int>(tileHeight)
            };
            
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        }
    }
}
