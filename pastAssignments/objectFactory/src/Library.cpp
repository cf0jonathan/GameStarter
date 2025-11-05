#include "Library.h"
#include "Object.h"
#include "Player.h"
#include "Crate.h"
#include "Enemy.h"
#include <iostream>

Library::Library() {
    std::cout << "[Library] Registering object types...\n";
    
    map["Player"] = Player::create;
    std::cout << "[Library] Registered: Player\n";
    
    map["Crate"] = Crate::create;
    std::cout << "[Library] Registered: Crate\n";
    
    map["Enemy"] = Enemy::create;
    std::cout << "[Library] Registered: Enemy\n";
    
    std::cout << "[Library] Registration complete. " << map.size() << " types available.\n";
}

std::unique_ptr<Object> Library::create(const std::string& type, const tinyxml2::XMLElement* xmlElement) {
    auto it = map.find(type);
    
    if (it != map.end()) {
        std::cout << "[Library] Creating object of type: " << type << "\n";
        return it->second(xmlElement);
    } else {
        std::cout << "[Library] ERROR: Unknown object type '" << type << "'\n";
        return nullptr;
    }
}

bool Library::hasType(const std::string& type) const {
    return map.find(type) != map.end();
}