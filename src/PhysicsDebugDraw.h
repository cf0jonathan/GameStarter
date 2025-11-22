#pragma once
#include <vector>
#include <memory>

class GameObject;
struct SDL_Renderer;

class PhysicsDebugDraw {
public:
    static void toggle();
    static void setEnabled(bool enabled);
    static bool isEnabled();

    static void render(SDL_Renderer* renderer, const std::vector<std::unique_ptr<GameObject>>& objects);

private:
    static bool enabled;
};
