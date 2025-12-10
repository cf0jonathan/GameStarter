#include "TextComponent.h"
#include <SDL.h>
#include <SDL_ttf.h>

TextComponent::TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, int x, int y)
    : text(text), fontPath(fontPath), fontSize(fontSize), color(color), x(x), y(y) {
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
}

TextComponent::~TextComponent() {
    if (texture) SDL_DestroyTexture(texture);
    if (font) TTF_CloseFont(font);
}

void TextComponent::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
}

void TextComponent::updateTexture() {
    if (!font || !renderer || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    destRect = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
}

void TextComponent::render() {
    if (!texture) updateTexture();
    if (texture && renderer) SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}
