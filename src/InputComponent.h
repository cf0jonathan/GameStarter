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
    
    // Mouse button state
    bool isMouseButtonPressed(int button) const;  // Just pressed this frame
    bool isMouseButtonDown(int button) const;     // Currently held
    bool isMouseButtonUp(int button) const;       // Currently not pressed
    
    // Convenience methods
    bool isLeftMouseDown() const { return isMouseButtonDown(SDL_BUTTON_LEFT); }
    bool isLeftMousePressed() const { return isMouseButtonPressed(SDL_BUTTON_LEFT); }
    bool isRightMouseDown() const { return isMouseButtonDown(SDL_BUTTON_RIGHT); }
    
private:
    const Uint8* currentKeyStates = nullptr;
    Uint8 previousKeyStates[SDL_NUM_SCANCODES] = {0};
    
    Uint32 currentMouseState = 0;
    Uint32 previousMouseState = 0;
};
