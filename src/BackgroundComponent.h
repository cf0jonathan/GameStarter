#pragma once
#include "Component.h"
#include <string>

class BackgroundComponent : public Component {
public:
    BackgroundComponent() = default;
    
    void render() override;
    
    void setTexture(const std::string& textureId) { this->textureId = textureId; }
    void setTileSize(float w, float h) { tileWidth = w; tileHeight = h; }
    void setScrollSpeed(float speedX, float speedY) { scrollSpeedX = speedX; scrollSpeedY = speedY; }
    
private:
    std::string textureId;
    float tileWidth = 800.0f;   // Default to screen size
    float tileHeight = 600.0f;
    float scrollSpeedX = 0.0f;  // Parallax scroll speed (0 = static, 1.0 = same as camera)
    float scrollSpeedY = 0.0f;
    float offsetX = 0.0f;       // Current scroll offset
    float offsetY = 0.0f;
};
