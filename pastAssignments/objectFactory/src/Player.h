#pragma once
#include "Object.h"
#include <memory>
#include <iostream>

namespace tinyxml2 {
    class XMLElement;
}

class Player : public Object {
public:
    Player() = default;
    
    void update() override {
    }
    
    void draw() override {
        std::cout << "Drawing Player at (" << x << ", " << y 
                  << ") angle=" << angle << " Degrees\n";
    }
    
    static std::unique_ptr<Object> create(const tinyxml2::XMLElement* xmlElement);
};