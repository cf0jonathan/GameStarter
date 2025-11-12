#pragma once
#include <SDL2/SDL.h>

class Controller {
public:
    static Controller& getInstance();
    
    void update();
    
    bool isKeyPressed(SDL_Scancode key) const;
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyUp(SDL_Scancode key) const;
    
    // Convenience methods for common keys
    bool isWPressed() const { return isKeyDown(SDL_SCANCODE_W); }
    bool isAPressed() const { return isKeyDown(SDL_SCANCODE_A); }
    bool isSPressed() const { return isKeyDown(SDL_SCANCODE_S); }
    bool isDPressed() const { return isKeyDown(SDL_SCANCODE_D); }
    
private:
    Controller() = default;
    ~Controller() = default;
    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;
    
    const Uint8* currentKeyStates = nullptr;
    Uint8 previousKeyStates[SDL_NUM_SCANCODES] = {0};
};
