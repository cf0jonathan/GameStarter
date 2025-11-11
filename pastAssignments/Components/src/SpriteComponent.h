#pragma once
#include "Component.h"
#include "BodyComponent.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include "Textures.h"

// Forward declaration to avoid circular include
class Engine;

class SpriteComponent : public Component {
public:
    SpriteComponent(GameObject& parent, std::string textureKey )
        : Component(parent), texture(nullptr) {
        texture = Textures::get(textureKey);
        if (!texture) {
            SDL_Log("Failed to load texture for key: %s", textureKey.c_str());
        }
    }
    void update() override {}

    void draw() override {
        auto body = parent().get<BodyComponent>();
        if (body) {
            SDL_Rect dst = { static_cast<int>(body->x), static_cast<int>(body->y), 64, 64 };
            // Get renderer from external source (defined in Engine.cpp)
            extern SDL_Renderer* getGameRenderer();
            SDL_RenderCopyEx(getGameRenderer(), texture, nullptr, &dst, body->angle, nullptr, SDL_FLIP_NONE);
        }
    }



private:
    SDL_Texture* texture;
};