#include "PhysicsDebugDraw.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "PhysicsBodyComponent.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include "View.h"
#include "Engine.h"

bool PhysicsDebugDraw::enabled = false;

void PhysicsDebugDraw::toggle() { enabled = !enabled; std::cout << "Physics debug draw " << (enabled ? "ENABLED" : "DISABLED") << std::endl; }
void PhysicsDebugDraw::setEnabled(bool e) { enabled = e; }
bool PhysicsDebugDraw::isEnabled() { return enabled; }

void PhysicsDebugDraw::render(SDL_Renderer* renderer, const std::vector<std::unique_ptr<GameObject>>& objects) {
    if (!enabled) return;
    if (!renderer) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int bodyCount = 0;
    for (const auto& obj : objects) {
        auto* phys = obj->getComponent<PhysicsBodyComponent>();
        if (!phys || !phys->isInitialized()) continue;
        auto* transform = obj->getComponent<TransformComponent>();
        auto* sprite = obj->getComponent<SpriteComponent>();
        if (!transform) continue;
        ++bodyCount;

        // World position
        float worldX = transform->getX();
        float worldY = transform->getY();
        // Reuse View::worldToScreen for consistency via a dummy rect
        SDL_Rect dummy{(int)worldX, (int)worldY, 0, 0};
        SDL_Rect screenPt = View::getInstance().worldToScreen(dummy);
        float x = (float)screenPt.x;
        float y = (float)screenPt.y;
        float angleDeg = transform->getRotation();
        float angleRad = angleDeg * 3.14159265f / 180.0f;

        // Choose color by body type
        Uint8 r=0,g=255,b=0,a=180; // dynamic default green
        switch (phys->getBodyType()) {
            case BodyType::Static: r=160; g=160; b=160; a=200; break;
            case BodyType::Kinematic: r=0; g=128; b=255; a=200; break;
            case BodyType::Dynamic: default: r=0; g=255; b=0; a=200; break;
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Dimensions from sprite (fallback 50x50)
        float width = 50.0f;
        float height = 50.0f;
        if (sprite) {
            width = sprite->getWidth();
            height = sprite->getHeight();
        }

        if (phys->getShapeType() == ShapeType::Circle) {
            float radius = std::max(width, height) * 0.5f; // already in pixels
            const int segments = 24;
            for (int i = 0; i < segments; ++i) {
                float a1 = (2.0f * 3.14159265f * i) / segments;
                float a2 = (2.0f * 3.14159265f * (i + 1)) / segments;
                int x1 = (int)(x + radius * std::cos(a1));
                int y1 = (int)(y + radius * std::sin(a1));
                int x2 = (int)(x + radius * std::cos(a2));
                int y2 = (int)(y + radius * std::sin(a2));
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
        } else { // Ellipse approximated polygon
            const int numVertices = 12;
            float hw = width * 0.5f; // pixels
            float hh = height * 0.5f;
            int prevX = 0, prevY = 0, firstX = 0, firstY = 0;
            for (int i = 0; i < numVertices; ++i) {
                float a = (2.0f * 3.14159265f * i) / numVertices;
                float lx = hw * std::cos(a);
                float ly = hh * std::sin(a);
                // rotate
                float rx = lx * std::cos(angleRad) - ly * std::sin(angleRad);
                float ry = lx * std::sin(angleRad) + ly * std::cos(angleRad);
                int vx = (int)(x + rx);
                int vy = (int)(y + ry);
                if (i > 0) SDL_RenderDrawLine(renderer, prevX, prevY, vx, vy);
                else { firstX = vx; firstY = vy; }
                prevX = vx; prevY = vy;
            }
            // close polygon
            SDL_RenderDrawLine(renderer, prevX, prevY, firstX, firstY);
        }

        // Draw center cross
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 200);
        SDL_RenderDrawLine(renderer, (int)x - 4, (int)y, (int)x + 4, (int)y);
        SDL_RenderDrawLine(renderer, (int)x, (int)y - 4, (int)x, (int)y + 4);

        // Optional: velocity vector (dynamic bodies only)
        if (phys->getBodyType() == BodyType::Dynamic) {
            b2BodyId bodyId = phys->getBodyId();
            b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
            float scale = 35.0f; // length multiplier
            int vx = (int)(x + vel.x * PhysicsBodyComponent::PIXELS_PER_METER * (scale * 0.016f));
            int vy = (int)(y + vel.y * PhysicsBodyComponent::PIXELS_PER_METER * (scale * 0.016f));
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 200);
            SDL_RenderDrawLine(renderer, (int)x, (int)y, vx, vy);
        }
    }
    if (bodyCount == 0) {
        // Helpful one-time message when enabled but no bodies found
        static bool warned = false;
        if (!warned) { std::cout << "Physics debug draw: no bodies found to render" << std::endl; warned = true; }
    }
}
