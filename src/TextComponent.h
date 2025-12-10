#pragma once
#include "Component.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class SDL_Renderer;

class TextComponent : public Component {
public:
    TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, int x, int y);
    ~TextComponent();
    void setText(const std::string& text);
    void setRenderer(SDL_Renderer* renderer) { this->renderer = renderer; }
    void render() override;
private:
    std::string text;
    std::string fontPath;
    int fontSize;
    SDL_Color color;
    int x, y;
    TTF_Font* font = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Rect destRect{};
    SDL_Renderer* renderer = nullptr;
    void updateTexture();
};
