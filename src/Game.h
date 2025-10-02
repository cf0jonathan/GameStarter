#pragma once
#include <memory>
#include <string>
#include <vector>
#include <random>
#include <SDL.h>
#include <box2d/box2d.h>

class Game {
public:
    int run();
private:
    bool init();
    void shutdown();
    void stepPhysics(float dt);
    void render();
    void spawnRandomBlock();
    bool loadConfig(const std::string& path);

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* blockTexture_ = nullptr;

    b2WorldId worldId_ = b2_nullWorldId;
    
    struct FallingBlock {
        b2BodyId bodyId;
        SDL_Color color;
    };
    std::vector<FallingBlock> fallingBlocks_;
    b2BodyId groundId_ = b2_nullBodyId;
    b2BodyId leftWallId_ = b2_nullBodyId;
    b2BodyId rightWallId_ = b2_nullBodyId;
    b2BodyId roofId_ = b2_nullBodyId;
    b2BodyId mouseBlockId_ = b2_nullBodyId; // Physics body for mouse-following block

    float gravityY_ = 9.8f;
    std::string title_ = "Falling Blocks";
    std::mt19937 rng_;
    float spawnTimer_ = 0.0f;
    float spawnInterval_ = 0.5f; // Spawn rate from config
    float boxSize_ = 0.5f; // Box size from config (falling blocks)
    float mouseBlockSize_ = 0.5f; // Mouse block size from config
    
    // Mouse-following block
    int mouseX_ = 400; // Mouse X position in screen coordinates
    int mouseY_ = 300; // Mouse Y position in screen coordinates
    
    // Rendering constants
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    static constexpr float PIXELS_PER_METER = 50.0f; // Scale factor
};
