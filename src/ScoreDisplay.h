#pragma once
#include "TextComponent.h"
#include <SDL.h>
#include <string>

class ScoreDisplay {
public:
    ScoreDisplay(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
    ~ScoreDisplay();
    void setScore(int score);
    void render();
private:
    int score = 0;
    TextComponent* textComponent = nullptr;
    std::string fontPath;
    int fontSize;
};
