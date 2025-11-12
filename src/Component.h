#pragma once
#include <memory>

class GameObject;

class Component {
public:
    virtual ~Component() = default;
    
    virtual void init() {}
    virtual void update(float deltaTime) {}
    virtual void render() {}
    
    void setOwner(GameObject* owner) { this->owner = owner; }
    GameObject* getOwner() const { return owner; }
    
protected:
    GameObject* owner = nullptr;
};
