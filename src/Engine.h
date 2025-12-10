#pragma once
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <string>
#include <map>
#include <memory>
#include <vector>

class GameObject;

class Engine {
public:
    static Engine& getInstance();
    
    bool init(const std::string& title, int width, int height);
    void run();
    void quit();
    void clean();
    
    SDL_Renderer* getRenderer() { return renderer; }
    bool isRunning() { return running; }
    
    // Window dimensions
    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }
    
    // Frame rate control
    static void setTargetFPS(int fps);
    static float getDeltaTime() { return deltaTime; }
    static void setLogicFPS(int fps);
    static int getLogicFPS() { return logicFPS; }
    static float getFixedDeltaTime() { return fixedDeltaTime; }
    
    // Mouse state
    static int getMouseX() { return mouseX; }
    static int getMouseY() { return mouseY; }
    
    // Physics
    b2WorldId getPhysicsWorld() const { return physicsWorldId; }
    
    // Asset loading
    void loadGameObjectsFromXML(const std::string& filepath);
    
    // Programmatic object spawning
    GameObject* spawnBackground(const std::string& texture, float tileW, float tileH, float scrollX, float scrollY);
    GameObject* spawnPlayer(float x, float y, const std::string& texture, float spriteWidth,
                           float thrustForce, float maxSpeed);
    GameObject* spawnAsteroid(float x, float y, float size);
    
    // Object lifecycle management
    void removeGameObject(GameObject* obj);
    void cleanupMarkedObjects();
    
    // Procedural generation (stubs for future use)
    void updateAsteroidSpawning();
    void cleanupOffscreenAsteroids();
    
    // Game object access (for components that need to iterate)
    std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return gameObjects; }
    
public:
    // Score display
    void setScore(int score);
    int getScore() const { return score; }
    bool isGameOver() const { return gameOver; }

private:
    int score = 0;
    class ScoreDisplay* scoreDisplay = nullptr;
    class GameOverScreen* gameOverScreen = nullptr;
    void updateAsteroidDifficulty();  // Update spawn rate based on score
    GameObject* player = nullptr;           // Reference to player for distance calculation
    float playerSpawnX = 0.0f, playerSpawnY = 0.0f;
    bool gameOver = false;
    struct ExplosionConfig {
        int burstCount = 60;
        float burstDuration = 0.18f;
        float lifetime = 0.9f;
        float size = 4.0f;
        float speedMin = 100.0f;
        float speedMax = 300.0f;
        float spreadAngle = 360.0f;
        float directionOffset = 0.0f;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        SDL_Color startColor = {255, 200, 100, 255};
        SDL_Color endColor = {100, 50, 0, 0};
    } explosionConfig;

    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    void handleEvents();
    void update();
    void render();
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    int windowWidth = 0;
    int windowHeight = 0;
    
    // Frame rate limiting
    static int targetFPS;
    static float deltaTime;
    static int logicFPS;          // Fixed update frequency (independent of render FPS)
    static float fixedDeltaTime;  // Fixed timestep in seconds
    Uint32 lastFrameTime = 0;
    
    // Mouse state
    static int mouseX;
    static int mouseY;
    
    // Physics
    b2WorldId physicsWorldId = b2_nullWorldId;
    
    // Game objects
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    
    // Asteroid spawn queue (processed after update loop to avoid vector reallocation issues)
    struct PendingAsteroid {
        float x, y, size;
    };
    std::vector<PendingAsteroid> pendingAsteroids;
    
    void processPendingAsteroids();  // Called after updates to spawn queued asteroids
};
