#pragma once
#include "Component.h"
#include <SDL2/SDL.h>

class InputComponent : public Component {
public:
    InputComponent() = default;
    
    void update(float deltaTime) override;
    
    bool isKeyPressed(SDL_Scancode key) const;
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyUp(SDL_Scancode key) const;
    
    // Convenience methods
    bool isWPressed() const { return isKeyDown(SDL_SCANCODE_W); }
    bool isAPressed() const { return isKeyDown(SDL_SCANCODE_A); }
    bool isSPressed() const { return isKeyDown(SDL_SCANCODE_S); }
    bool isDPressed() const { return isKeyDown(SDL_SCANCODE_D); }
    
private:
    const Uint8* currentKeyStates = nullptr;
    Uint8 previousKeyStates[SDL_NUM_SCANCODES] = {0};
};
