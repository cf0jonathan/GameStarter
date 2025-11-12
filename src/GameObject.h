#pragma once
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

class Component;

class GameObject {
public:
    GameObject() = default;
    ~GameObject() = default;
    
    void update(float deltaTime);
    void render();
    
    template<typename T>
    T* addComponent() {
        auto component = std::make_unique<T>();
        component->setOwner(this);
        T* ptr = component.get();
        components.push_back(std::move(component));
        componentMap[std::type_index(typeid(T))] = ptr;
        ptr->init();
        return ptr;
    }
    
    template<typename T>
    T* getComponent() {
        auto it = componentMap.find(std::type_index(typeid(T)));
        if (it != componentMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
private:
    std::vector<std::unique_ptr<Component>> components;
    std::unordered_map<std::type_index, Component*> componentMap;
};
