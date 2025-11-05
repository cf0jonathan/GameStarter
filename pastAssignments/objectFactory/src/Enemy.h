#pragma once
#include "Object.h"
#include <memory>
#include <iostream>

namespace tinyxml2 {
    class XMLElement;
}

class Enemy : public Object {
public:
    Enemy() = default;
    
    void update() override {
    }
    
    void draw() override {
        std::cout << "Drawing Enemy at (" << x << ", " << y 
                  << ") angle=" << angle << " Degrees\n";
    }
    
    static std::unique_ptr<Object> create(const tinyxml2::XMLElement* xmlElement);
};