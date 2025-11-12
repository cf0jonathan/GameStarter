#pragma once
#include "Component.h"

class TransformComponent;

class CameraFollowComponent : public Component {
public:
    CameraFollowComponent() = default;
    
    void init() override;
    void update(float deltaTime) override;
    
private:
    TransformComponent* transform = nullptr;
};
