#include "CameraFollowComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "View.h"

void CameraFollowComponent::init() {
    transform = owner->getComponent<TransformComponent>();
}

void CameraFollowComponent::update(float deltaTime) {
    if (!transform) return;
    
    View::getInstance().setCenter(transform->getX(), transform->getY());
}
