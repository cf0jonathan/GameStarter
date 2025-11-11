#pragma once

class GameObject;  // Forward declaration

class Component {
public:
    Component(GameObject& parent) : _parent(parent) {}
    
    virtual void update() = 0;  // Pure virtual - must be implemented
    virtual void draw() = 0;    // Pure virtual - must be implemented
    
    virtual ~Component() = default;

protected:
    GameObject& parent() const { return _parent; }  // Access to parent GameObject

private:
    GameObject& _parent;  // Reference to the GameObject that owns this component
};
