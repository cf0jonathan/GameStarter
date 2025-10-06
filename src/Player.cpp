#include "Player.h"
#include <tinyxml2.h>

std::unique_ptr<Object> Player::create(const tinyxml2::XMLElement* xmlElement) {
    auto player = std::make_unique<Player>();
    
    if (xmlElement) {
        const auto* bodyElement = xmlElement->FirstChildElement("Body");
        if (bodyElement) {
            float x = bodyElement->FloatAttribute("x", 0.0f);
            float y = bodyElement->FloatAttribute("y", 0.0f);  
            float angle = bodyElement->FloatAttribute("angle", 0.0f);
            
            player->setPosition(x, y, angle);
            
            std::cout << "[Factory] Created Player at (" << x << ", " << y 
                      << ") angle=" << angle << " Degrees\n";
        }
    }
    
    return player;
}