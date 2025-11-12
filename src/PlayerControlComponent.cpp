#include "PlayerControlComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "InputComponent.h"

void PlayerControlComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    input = owner->getComponent<InputComponent>();
}

void PlayerControlComponent::update(float deltaTime) {
    if (!transform || !input) return;
    
    float velX = 0;
    float velY = 0;
    
    if (input->isWPressed()) {
        velY = -speed;
    }
    if (input->isSPressed()) {
        velY = speed;
    }
    if (input->isAPressed()) {
        velX = -speed;
    }
    if (input->isDPressed()) {
        velX = speed;
    }
    
    transform->setVelocity(velX, velY);
}
