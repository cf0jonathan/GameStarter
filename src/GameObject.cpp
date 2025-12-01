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

void GameObject::setTag(const std::string& newTag) {
    tag = newTag;
}

const std::string& GameObject::getTag() const {
    return tag;
}

bool GameObject::hasTag(const std::string& tagToCheck) const {
    return tag == tagToCheck;
}

void GameObject::markForDeletion() {
    markedForDeletion = true;
}

bool GameObject::isMarkedForDeletion() const {
    return markedForDeletion;
}
