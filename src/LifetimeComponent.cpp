#include "LifetimeComponent.h"
#include "GameObject.h"

void LifetimeComponent::update(float deltaTime) {
    remaining -= deltaTime;
    if (remaining <= 0.0f) {
        owner->markForDeletion();
    }
}
