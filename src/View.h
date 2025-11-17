#pragma once
#include <SDL2/SDL.h>

class View {
public:
    static View& getInstance();
    
    void setCenter(float x, float y);
    void getCenter(float& x, float& y) const;
    
    // Transform world coordinates to screen coordinates
    SDL_Rect worldToScreen(const SDL_Rect& worldRect) const;
    
    // Transform screen coordinates to world coordinates
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;
    
    // Optional: scale and rotation
    void setScale(float s) { scale = s; }
    float getScale() const { return scale; }
    
private:
    View() = default;
    ~View() = default;
    View(const View&) = delete;
    View& operator=(const View&) = delete;
    
    float centerX = 0.0f;
    float centerY = 0.0f;
    float scale = 1.0f;
};
