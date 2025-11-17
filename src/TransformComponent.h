#pragma once
#include "Component.h"

class TransformComponent : public Component {
public:
    TransformComponent() = default;
    
    void setPosition(float x, float y) {
        this->x = x;
        this->y = y;
    }
    
    void setVelocity(float vx, float vy) {
        this->velocityX = vx;
        this->velocityY = vy;
    }
    
    void setRotation(float angleDegrees) {
        this->rotation = angleDegrees;
    }
    
    void update(float deltaTime) override {
        x += velocityX * deltaTime;
        y += velocityY * deltaTime;
    }
    
    float getX() const { return x; }
    float getY() const { return y; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    float getRotation() const { return rotation; }
    
private:
    float x = 0.0f;
    float y = 0.0f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float rotation = 0.0f;  // Angle in degrees (0 = right)
};
