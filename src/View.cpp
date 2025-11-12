#include "View.h"

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
    // Assuming screen center is 400, 300 (800x600 / 2)
    // Transform world coordinates relative to view center
    SDL_Rect screenRect;
    screenRect.x = static_cast<int>((worldRect.x - centerX) * scale + 400);
    screenRect.y = static_cast<int>((worldRect.y - centerY) * scale + 300);
    screenRect.w = static_cast<int>(worldRect.w * scale);
    screenRect.h = static_cast<int>(worldRect.h * scale);
    return screenRect;
}
