#include "View.h"
#include "Engine.h"

View& View::getInstance() {
    static View instance;
    return instance;
}

void View::setCenter(float x, float y) {
    centerX = x;
    centerY = y;
}

void View::getCenter(float& x, float& y) const {
    x = centerX;
    y = centerY;
}

SDL_Rect View::worldToScreen(const SDL_Rect& worldRect) const {
    // Get window dimensions from Engine
    float screenCenterX = Engine::getInstance().getWindowWidth() / 2.0f;
    float screenCenterY = Engine::getInstance().getWindowHeight() / 2.0f;
    
    // Transform world coordinates relative to view center
    SDL_Rect screenRect;
    screenRect.x = static_cast<int>((worldRect.x - centerX) * scale + screenCenterX);
    screenRect.y = static_cast<int>((worldRect.y - centerY) * scale + screenCenterY);
    screenRect.w = static_cast<int>(worldRect.w * scale);
    screenRect.h = static_cast<int>(worldRect.h * scale);
    return screenRect;
}

void View::screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const {
    // Get window dimensions from Engine
    float screenCenterX = Engine::getInstance().getWindowWidth() / 2.0f;
    float screenCenterY = Engine::getInstance().getWindowHeight() / 2.0f;
    
    // Inverse transform
    worldX = (screenX - screenCenterX) / scale + centerX;
    worldY = (screenY - screenCenterY) / scale + centerY;
}
