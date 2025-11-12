#pragma once
#include "Component.h"

class TransformComponent;
class InputComponent;

class PlayerControlComponent : public Component {
public:
    PlayerControlComponent() = default;
    
    void init() override;
    void update(float deltaTime) override;
    
    void setSpeed(float speed) { this->speed = speed; }
    
private:
    TransformComponent* transform = nullptr;
    InputComponent* input = nullptr;
    float speed = 200.0f;
};
