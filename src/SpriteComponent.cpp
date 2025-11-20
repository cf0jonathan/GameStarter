#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "View.h"
#include "AssetManager.h"

void SpriteComponent::init() {
    transform = owner->getComponent<TransformComponent>();
}

void SpriteComponent::setSizePreserveAspect(float size, bool useWidth) {
    if (textureId.empty()) {
        // No texture, just set both to size
        width = height = size;
        return;
    }
    
    int texWidth, texHeight;
    if (AssetManager::getInstance().getTextureDimensions(textureId, texWidth, texHeight)) {
        float aspectRatio = static_cast<float>(texWidth) / static_cast<float>(texHeight);
        if (useWidth) {
            width = size;
            height = size / aspectRatio;
        } else {
            height = size;
            width = size * aspectRatio;
        }
    } else {
        // Couldn't get dimensions, fall back to square
        width = height = size;
    }
}

void SpriteComponent::render() {
    if (!transform) return;
    
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    
    // Create world space rectangle centered on transform position
    SDL_Rect worldRect = {
        static_cast<int>(transform->getX() - width / 2.0f),
        static_cast<int>(transform->getY() - height / 2.0f),
        static_cast<int>(width),
        static_cast<int>(height)
    };
    
    // Transform to screen space using View
    SDL_Rect screenRect = View::getInstance().worldToScreen(worldRect);
    
    // Get rotation from transform
    double angle = transform->getRotation();
    
    // Draw with texture if available, otherwise use color
    if (!textureId.empty()) {
        SDL_Texture* texture = AssetManager::getInstance().getTexture(textureId);
        if (texture) {
            // Center point for rotation in screen space (middle of sprite)
            SDL_Point center = { screenRect.w / 2, screenRect.h / 2 };
            SDL_RenderCopyEx(renderer, texture, nullptr, &screenRect, angle, &center, SDL_FLIP_NONE);
            return;
        }
    }
    
    // Fallback to colored rectangle (rotation not supported for colored rects)
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &screenRect);
}
