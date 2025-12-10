#include "ScoreDisplay.h"
#include "TextComponent.h"
#include <string>

ScoreDisplay::ScoreDisplay(SDL_Renderer* renderer, const std::string& fontPath, int fontSize)
    : fontPath(fontPath), fontSize(fontSize) {
    SDL_Color color = {255, 255, 255, 255};
    textComponent = new TextComponent("Score: 0", fontPath, fontSize, color, 10, 10);
    textComponent->setRenderer(renderer);
}

ScoreDisplay::~ScoreDisplay() {
    delete textComponent;
}

void ScoreDisplay::setScore(int newScore) {
    score = newScore;
    textComponent->setText("Score: " + std::to_string(score));
}

void ScoreDisplay::render() {
    textComponent->render();
}
