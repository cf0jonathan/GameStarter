#include "Enemy.h"
#include <tinyxml2.h>

std::unique_ptr<Object> Enemy::create(const tinyxml2::XMLElement* xmlElement) {
    auto enemy = std::make_unique<Enemy>();
    
    if (xmlElement) {
        const auto* bodyElement = xmlElement->FirstChildElement("Body");
        if (bodyElement) {
            float x = bodyElement->FloatAttribute("x", 0.0f);
            float y = bodyElement->FloatAttribute("y", 0.0f);  
            float angle = bodyElement->FloatAttribute("angle", 0.0f);
            
            enemy->setPosition(x, y, angle);
            
            std::cout << "[Factory] Created Enemy at (" << x << ", " << y 
                      << ") angle=" << angle << " Degrees\n";
        }
    }
    
    return enemy;
}