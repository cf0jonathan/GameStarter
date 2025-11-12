#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "View.h"
#include "AssetManager.h"

void SpriteComponent::init() {
    transform = owner->getComponent<TransformComponent>();
}

void SpriteComponent::render() {
    if (!transform) return;
    
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    
    // Create world space rectangle
    SDL_Rect worldRect = {
        static_cast<int>(transform->getX()),
        static_cast<int>(transform->getY()),
        static_cast<int>(width),
        static_cast<int>(height)
    };
    
    // Transform to screen space using View
    SDL_Rect screenRect = View::getInstance().worldToScreen(worldRect);
    
    // Draw with texture if available, otherwise use color
    if (!textureId.empty()) {
        SDL_Texture* texture = AssetManager::getInstance().getTexture(textureId);
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &screenRect);
            return;
        }
    }
    
    // Fallback to colored rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &screenRect);
}
