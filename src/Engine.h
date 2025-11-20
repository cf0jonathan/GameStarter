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
    
private:
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
};
