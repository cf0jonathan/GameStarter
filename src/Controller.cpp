#include "Controller.h"
#include <cstring>

Controller& Controller::getInstance() {
    static Controller instance;
    return instance;
}

void Controller::update() {
    // Save previous state
    if (currentKeyStates) {
        std::memcpy(previousKeyStates, currentKeyStates, SDL_NUM_SCANCODES);
    }
    
    // Get current state
    currentKeyStates = SDL_GetKeyboardState(nullptr);
}

bool Controller::isKeyPressed(SDL_Scancode key) const {
    // Just pressed this frame (was up, now down)
    return currentKeyStates && currentKeyStates[key] && !previousKeyStates[key];
}

bool Controller::isKeyDown(SDL_Scancode key) const {
    // Currently held down
    return currentKeyStates && currentKeyStates[key];
}

bool Controller::isKeyUp(SDL_Scancode key) const {
    // Currently not pressed
    return currentKeyStates && !currentKeyStates[key];
}
