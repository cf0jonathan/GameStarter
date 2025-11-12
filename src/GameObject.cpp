#include "GameObject.h"
#include "Component.h"

void GameObject::update(float deltaTime) {
    for (auto& component : components) {
        component->update(deltaTime);
    }
}

void GameObject::render() {
    for (auto& component : components) {
        component->render();
    }
}
