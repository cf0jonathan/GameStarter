#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <string>

class TransformComponent;

class SpriteComponent : public Component {
public:
    SpriteComponent() = default;
    
    void init() override;
    void render() override;
    
    void setTexture(const std::string& textureId) { this->textureId = textureId; }
    void setSize(float w, float h) { width = w; height = h; }
    void setSizePreserveAspect(float size, bool useWidth = true);
    void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    
private:
    TransformComponent* transform = nullptr;
    std::string textureId;
    float width = 50.0f;
    float height = 50.0f;
    Uint8 r = 255, g = 255, b = 255, a = 255;
};
