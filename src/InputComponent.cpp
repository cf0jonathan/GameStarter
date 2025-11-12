#include "InputComponent.h"
#include <cstring>
#include <cstdio>

void InputComponent::update(float deltaTime) {
    // Save previous state
    if (currentKeyStates) {
        std::memcpy(previousKeyStates, currentKeyStates, SDL_NUM_SCANCODES);
    }
    
    // Get current state
    currentKeyStates = SDL_GetKeyboardState(nullptr);
    
}

bool InputComponent::isKeyPressed(SDL_Scancode key) const {
    // Just pressed this frame (was up, now down)
    return currentKeyStates && currentKeyStates[key] && !previousKeyStates[key];
}

bool InputComponent::isKeyDown(SDL_Scancode key) const {
    // Currently held down
    return currentKeyStates && currentKeyStates[key];
}

bool InputComponent::isKeyUp(SDL_Scancode key) const {
    // Currently not pressed
    return currentKeyStates && !currentKeyStates[key];
}
