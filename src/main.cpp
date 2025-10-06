#include "Engine.h"
#include "Object.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Factory Pattern Game Engine Demo ===\n\n";
    
    std::string levelPath = "assets/level.xml";
    
    std::cout << "Creating engine and loading level...\n";
    Engine engine(levelPath);
    
    std::cout << "\n=== Engine Created Successfully ===\n";
    std::cout << "Objects loaded: " << engine.getObjectCount() << "\n\n";
    
    std::cout << "=== Object Details ===\n";
    for (int i = 0; i < engine.objects.size(); ++i) {
        std::cout << "[Main] Object #" << i
                  << " position=(" << engine.objects[i]->getX()
                  << ", " << engine.objects[i]->getY() << ")"
                  << " angle=" << engine.objects[i]->getAngle() << " Degrees\n";
    }
    
    std::cout << "\n=== Testing Game Loop Methods ===\n";
    
    std::cout << "--- Update Phase ---\n";
    engine.update();
    
    std::cout << "\n--- Draw Phase ---\n";
    engine.draw();

    return 0;
}