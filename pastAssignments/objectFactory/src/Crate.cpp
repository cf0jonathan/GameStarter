#include "Crate.h"
#include <tinyxml2.h>

std::unique_ptr<Object> Crate::create(const tinyxml2::XMLElement* xmlElement) {
    auto crate = std::make_unique<Crate>();
    
    if (xmlElement) {
        const auto* bodyElement = xmlElement->FirstChildElement("Body");
        if (bodyElement) {
            float x = bodyElement->FloatAttribute("x", 0.0f);
            float y = bodyElement->FloatAttribute("y", 0.0f);  
            float angle = bodyElement->FloatAttribute("angle", 0.0f);
            
            crate->setPosition(x, y, angle);
            
            std::cout << "[Factory] Created Crate at (" << x << ", " << y 
                      << ") angle=" << angle << " Degrees\n";
        }
    }
    
    return crate;
}