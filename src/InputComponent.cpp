#include "InputComponent.h"
#include <cstring>
#include <cstdio>

void InputComponent::update(float deltaTime) {
    // Save previous state
    if (currentKeyStates) {
        std::memcpy(previousKeyStates, currentKeyStates, SDL_NUM_SCANCODES);
    }
    previousMouseState = currentMouseState;
    
    // Get current state
    currentKeyStates = SDL_GetKeyboardState(nullptr);
    currentMouseState = SDL_GetMouseState(nullptr, nullptr);
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

bool InputComponent::isMouseButtonPressed(int button) const {
    // Just pressed this frame (was up, now down)
    Uint32 buttonMask = SDL_BUTTON(button);
    return (currentMouseState & buttonMask) && !(previousMouseState & buttonMask);
}

bool InputComponent::isMouseButtonDown(int button) const {
    // Currently held down
    return (currentMouseState & SDL_BUTTON(button)) != 0;
}

bool InputComponent::isMouseButtonUp(int button) const {
    // Currently not pressed
    return (currentMouseState & SDL_BUTTON(button)) == 0;
}
