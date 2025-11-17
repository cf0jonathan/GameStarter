#pragma once
#include "Component.h"

class TransformComponent;

class RotateToMouseComponent : public Component {
public:
    RotateToMouseComponent() = default;
    
    void init() override;
    void update(float deltaTime) override;
    
private:
    TransformComponent* transform = nullptr;
};
