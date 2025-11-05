#pragma once
#include "Library.h"
#include "Object.h"  // Need full Object definition for unique_ptr<Object> vector
#include <vector>
#include <memory>
#include <string>

struct Engine {
    Library objectLibrary;
    std::vector<std::unique_ptr<Object>> objects;
    
    Engine();
    Engine(const std::string& levelPath);
    
    void loadLevel(const std::string& levelPath);
    void update();
    void draw();
    
    size_t getObjectCount() const { return objects.size(); }
    void clearLevel() { objects.clear(); }
};