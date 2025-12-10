#include "Engine.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "CameraFollowComponent.h"
#include "RotateToMouseComponent.h"
#include "MoveComponent.h"
#include "BackgroundComponent.h"
#include "PhysicsBodyComponent.h"
#include "View.h"
#include "PhysicsDebugDraw.h"
#include "ParticleSystem.h"
#include "ParticleEmitterComponent.h"
#include "LifetimeComponent.h"
#include "AsteroidSpawner.h"
#include <tinyxml2.h>
#include <iostream>
#include <cmath>
#include <SDL2/SDL_mixer.h>
#include "ScoreDisplay.h"
#include "GameOverScreen.h"
#include "AssetManager.h"

int Engine::targetFPS = 60;
float Engine::deltaTime = 0.0f;
int Engine::mouseX = 0;
int Engine::mouseY = 0;
int Engine::logicFPS = 60;
float Engine::fixedDeltaTime = 1.0f / 60.0f;

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::init(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    windowWidth = width;
    windowHeight = height;
    
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_mixer with MP3 support
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        std::cerr << "SDL_mixer could not initialize MP3 support! SDL_mixer Error: " << Mix_GetError() << std::endl;
        // Continue anyway, maybe WAV will work
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Create score display (use a default font path, e.g., assets/arial.ttf)
    scoreDisplay = new ScoreDisplay(renderer, "assets/arial.ttf", 24);
    
    // Create game over screen
    gameOverScreen = new GameOverScreen(renderer, "assets/arial.ttf", 48, width, height);
    
    // Load high score from XML
    AssetManager::getInstance().loadHighScoreFromXML("assets/config.xml");
    gameOverScreen->setHighScore(AssetManager::getInstance().getHighScore());
    
    // Create physics world with zero gravity (space environment)
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f};
    physicsWorldId = b2CreateWorld(&worldDef);
    
    running = true;
    lastFrameTime = SDL_GetTicks();
    
    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

GameObject* Engine::spawnBackground(const std::string& texture, float tileW, float tileH, float scrollX, float scrollY) {
    auto gameObj = std::make_unique<GameObject>();
    gameObj->setTag("background");
    
    auto* background = gameObj->addComponent<BackgroundComponent>();
    background->setTexture(texture);
    background->setTileSize(tileW, tileH);
    background->setScrollSpeed(scrollX, scrollY);
    
    GameObject* ptr = gameObj.get();
    gameObjects.push_back(std::move(gameObj));
    return ptr;
}

GameObject* Engine::spawnPlayer(float x, float y, const std::string& texture, float spriteWidth,
                                float thrustForce, float maxSpeed) {
    auto gameObj = std::make_unique<GameObject>();
    gameObj->setTag("player");
    
    // Transform
    auto* transform = gameObj->addComponent<TransformComponent>();
    transform->setPosition(x, y);
    
    // Sprite
    auto* sprite = gameObj->addComponent<SpriteComponent>();
    sprite->setTexture(texture);
    sprite->setSizePreserveAspect(spriteWidth, true);
    
    // Physics (hardcoded for player - dynamic ellipse with specific settings)
    auto* physics = gameObj->addComponent<PhysicsBodyComponent>();
    physics->setBodyType(BodyType::Dynamic);
    physics->setShapeType(ShapeType::Ellipse);
    physics->setLinearDamping(0.8f);
    physics->setAngularDamping(0.0f);
    physics->setFixedRotation(false);
    physics->setDensity(1.0f);
    
    // Input & Control
    gameObj->addComponent<InputComponent>();
    
    // Rotation behavior (hardcoded defaults - can be parameterized later if needed)
    auto* rotateToMouse = gameObj->addComponent<RotateToMouseComponent>();
    rotateToMouse->setMaxAngularSpeed(360.0f);
    rotateToMouse->setAcceleration(720.0f);
    rotateToMouse->setDeceleration(1080.0f);
    rotateToMouse->setSlowRadius(60.0f);
    rotateToMouse->setDeadZone(1.0f);
    rotateToMouse->setEaseExponent(0.6f);
    
    // Movement
    auto* move = gameObj->addComponent<MoveComponent>();
    move->setThrustForce(thrustForce);
    move->setMaxSpeed(maxSpeed);
    
    // Camera tracking
    gameObj->addComponent<CameraFollowComponent>();
    
    // Particle emitter (engine trail)
    auto* emitter = gameObj->addComponent<ParticleEmitterComponent>();
    emitter->setEmitterType(EmitterType::Continuous);
    emitter->setSpawnRate(50.0f);
    emitter->setLifetime(0.5f);
    emitter->setSpeedMin(50.0f);
    emitter->setSpeedMax(150.0f);
    emitter->setSize(3.0f);
    emitter->setSpreadAngle(30.0f);
    emitter->setDirectionOffset(180.0f);  // Behind the rocket
    emitter->setPositionOffset(-30.0f, 0.0f);  // Offset to rear
    emitter->setRequireMouseButton(true);  // Only emit when thrusting
    SDL_Color trailStart = {255, 128, 0, 255};  // orange
    SDL_Color trailEnd = {255, 0, 0, 0};        // red fading out
    emitter->setStartColor(trailStart);
    emitter->setEndColor(trailEnd);
    
    // Asteroid spawner (manages procedural asteroid generation via spawn queue)
    auto* spawner = gameObj->addComponent<AsteroidSpawner>();
    spawner->setSpawnBuffer(500.0f);      // spawn 500px beyond screen edge
    spawner->setCleanupDistance(1500.0f); // despawn 1500px from player
    spawner->setSpawnDensity(0.25f);      // 0.25 asteroids per direction (1 per 4 spawn triggers)
    
    GameObject* ptr = gameObj.get();
    gameObjects.push_back(std::move(gameObj));
    
    // Store player reference and spawn position for score tracking
    player = ptr;
    auto* playerTransform = player->getComponent<TransformComponent>();
    if (playerTransform) {
        playerSpawnX = playerTransform->getX();
        playerSpawnY = playerTransform->getY();
    }
    
    return ptr;
}

GameObject* Engine::spawnAsteroid(float x, float y, float size) {
    // Queue the asteroid for spawning after the update loop completes
    // This prevents vector reallocation crashes during component updates
    pendingAsteroids.push_back({x, y, size});
    return nullptr;  // Placeholder - actual object will exist after processPendingAsteroids()
}

void Engine::loadGameObjectsFromXML(const std::string& filepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load game objects XML: " << filepath << std::endl;
        return;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("assets");
    if (!root) return;
    
    tinyxml2::XMLElement* gameObjectsElement = root->FirstChildElement("gameObjects");
    if (!gameObjectsElement) return;
    
    for (tinyxml2::XMLElement* objElement = gameObjectsElement->FirstChildElement("object");
         objElement != nullptr;
         objElement = objElement->NextSiblingElement("object")) {
        
        const char* type = objElement->Attribute("type");
        if (!type) continue;
        
        std::string typeStr(type);
        
        // Spawn background
        if (typeStr == "background") {
            tinyxml2::XMLElement* bgElement = objElement->FirstChildElement("background");
            if (bgElement) {
                const char* texture = bgElement->Attribute("texture");
                float tileWidth = bgElement->FloatAttribute("tileWidth", 800.0f);
                float tileHeight = bgElement->FloatAttribute("tileHeight", 600.0f);
                float scrollX = bgElement->FloatAttribute("scrollX", 0.0f);
                float scrollY = bgElement->FloatAttribute("scrollY", 0.0f);
                
                spawnBackground(texture ? texture : "", tileWidth, tileHeight, scrollX, scrollY);
            }
            continue;
        }
        
        // Parse common attributes
        float x = 0.0f, y = 0.0f;
        tinyxml2::XMLElement* posElement = objElement->FirstChildElement("position");
        if (posElement) {
            x = posElement->FloatAttribute("x", 0.0f);
            y = posElement->FloatAttribute("y", 0.0f);
        }
        
        float spriteWidth = 50.0f;
        tinyxml2::XMLElement* spriteElement = objElement->FirstChildElement("sprite");
        if (spriteElement && spriteElement->Attribute("width")) {
            spriteWidth = spriteElement->FloatAttribute("width");
        }
        
        // Spawn player
        if (typeStr == "player") {
            tinyxml2::XMLElement* spriteElem = objElement->FirstChildElement("sprite");
            const char* texture = spriteElem ? spriteElem->Attribute("texture") : "rocket";
            
            tinyxml2::XMLElement* moveElement = objElement->FirstChildElement("move");
            if (!moveElement) moveElement = objElement->FirstChildElement("moveToMouse");
            float thrustForce = moveElement ? moveElement->FloatAttribute("thrustForce", 250.0f) : 250.0f;
            float maxSpeed = moveElement ? moveElement->FloatAttribute("maxSpeed", 450.0f) : 450.0f;
            
            GameObject* player = spawnPlayer(x, y, texture ? texture : "rocket", spriteWidth, thrustForce, maxSpeed);
            
            // Find player by tag (safer than holding pointer across potential reallocations)
            GameObject* playerPtr = nullptr;
            for (auto& obj : gameObjects) {
                if (obj && obj->hasTag("player")) {
                    playerPtr = obj.get();
                    break;
                }
            }
            
            if (!playerPtr) continue;
            
            // Parse particle emitter (if present)
            tinyxml2::XMLElement* particleElem = objElement->FirstChildElement("particleEmitter");
            if (particleElem) {
                auto* emitter = playerPtr->getComponent<ParticleEmitterComponent>();
                if (emitter) {
                    const char* typeAttr = particleElem->Attribute("type");
                    if (typeAttr && std::string(typeAttr) == "burst") {
                        emitter->setEmitterType(EmitterType::Burst);
                    }
                    
                    if (particleElem->Attribute("spawnRate")) {
                        emitter->setSpawnRate(particleElem->FloatAttribute("spawnRate"));
                    }
                    if (particleElem->Attribute("lifetime")) {
                        emitter->setLifetime(particleElem->FloatAttribute("lifetime"));
                    }
                    if (particleElem->Attribute("speedMin")) {
                        emitter->setSpeedMin(particleElem->FloatAttribute("speedMin"));
                    }
                    if (particleElem->Attribute("speedMax")) {
                        emitter->setSpeedMax(particleElem->FloatAttribute("speedMax"));
                    }
                    if (particleElem->Attribute("size")) {
                        emitter->setSize(particleElem->FloatAttribute("size"));
                    }
                    if (particleElem->Attribute("spreadAngle")) {
                        emitter->setSpreadAngle(particleElem->FloatAttribute("spreadAngle"));
                    }
                    if (particleElem->Attribute("directionOffset")) {
                        emitter->setDirectionOffset(particleElem->FloatAttribute("directionOffset"));
                    }
                    if (particleElem->Attribute("offsetX") && particleElem->Attribute("offsetY")) {
                        emitter->setPositionOffset(
                            particleElem->FloatAttribute("offsetX"),
                            particleElem->FloatAttribute("offsetY")
                        );
                    }
                    if (particleElem->Attribute("requireMouseButton")) {
                        emitter->setRequireMouseButton(particleElem->BoolAttribute("requireMouseButton", false));
                    }
                    if (particleElem->Attribute("burstCount")) {
                        emitter->setBurstCount(particleElem->IntAttribute("burstCount"));
                    }
                    if (particleElem->Attribute("burstDuration")) {
                        emitter->setBurstDuration(particleElem->FloatAttribute("burstDuration"));
                    }
                    
                    // Parse colors (format: "r,g,b,a")
                    const char* startColorStr = particleElem->Attribute("startColor");
                    if (startColorStr) {
                        int r, g, b, a;
                        if (sscanf(startColorStr, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
                            emitter->setStartColor({(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a});
                        }
                    }
                    const char* endColorStr = particleElem->Attribute("endColor");
                    if (endColorStr) {
                        int r, g, b, a;
                        if (sscanf(endColorStr, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
                            emitter->setEndColor({(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a});
                        }
                    }
                }
            }

            // Parse explosion emitter settings (optional) for collision effects
            tinyxml2::XMLElement* explosionElem = objElement->FirstChildElement("explosionEmitter");
            if (explosionElem) {
                if (explosionElem->Attribute("burstCount")) {
                    explosionConfig.burstCount = explosionElem->IntAttribute("burstCount", explosionConfig.burstCount);
                }
                if (explosionElem->Attribute("burstDuration")) {
                    explosionConfig.burstDuration = explosionElem->FloatAttribute("burstDuration", explosionConfig.burstDuration);
                }
                if (explosionElem->Attribute("lifetime")) {
                    explosionConfig.lifetime = explosionElem->FloatAttribute("lifetime", explosionConfig.lifetime);
                }
                if (explosionElem->Attribute("size")) {
                    explosionConfig.size = explosionElem->FloatAttribute("size", explosionConfig.size);
                }
                if (explosionElem->Attribute("speedMin")) {
                    explosionConfig.speedMin = explosionElem->FloatAttribute("speedMin", explosionConfig.speedMin);
                }
                if (explosionElem->Attribute("speedMax")) {
                    explosionConfig.speedMax = explosionElem->FloatAttribute("speedMax", explosionConfig.speedMax);
                }
                if (explosionElem->Attribute("spreadAngle")) {
                    explosionConfig.spreadAngle = explosionElem->FloatAttribute("spreadAngle", explosionConfig.spreadAngle);
                }
                if (explosionElem->Attribute("directionOffset")) {
                    explosionConfig.directionOffset = explosionElem->FloatAttribute("directionOffset", explosionConfig.directionOffset);
                }
                if (explosionElem->Attribute("offsetX")) {
                    explosionConfig.offsetX = explosionElem->FloatAttribute("offsetX", explosionConfig.offsetX);
                }
                if (explosionElem->Attribute("offsetY")) {
                    explosionConfig.offsetY = explosionElem->FloatAttribute("offsetY", explosionConfig.offsetY);
                }
                const char* expStartColor = explosionElem->Attribute("startColor");
                if (expStartColor) {
                    int r, g, b, a;
                    if (sscanf(expStartColor, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
                        explosionConfig.startColor = {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
                    }
                }
                const char* expEndColor = explosionElem->Attribute("endColor");
                if (expEndColor) {
                    int r, g, b, a;
                    if (sscanf(expEndColor, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
                        explosionConfig.endColor = {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
                    }
                }
            }
            
            continue;
        }
        
        // Spawn asteroid
        if (typeStr == "asteroid") {
            spawnAsteroid(x, y, spriteWidth);
            continue;
        }
    }
    
    std::cout << "Loaded " << gameObjects.size() << " game objects from XML" << std::endl;
}

void Engine::run() {
    const int frameDelay = 1000 / targetFPS; // render cap
    Uint32 previousTicks = SDL_GetTicks();
    float accumulator = 0.0f;
    
    while (running) {
        Uint32 currentTicks = SDL_GetTicks();
        float frameTime = (currentTicks - previousTicks) / 1000.0f; // seconds
        previousTicks = currentTicks;
        
        // Clamp very large frame time to avoid spiral of death
        if (frameTime > 0.25f) frameTime = 0.25f;
        accumulator += frameTime;
        
        handleEvents();
        
        // Fixed timestep updates
        while (accumulator >= fixedDeltaTime) {
            deltaTime = fixedDeltaTime; // expose for components querying getDeltaTime()
            update();
            accumulator -= fixedDeltaTime;
        }
        
        render();
        
        // Frame limiting for rendering only
        Uint32 afterRenderTicks = SDL_GetTicks();
        Uint32 loopFrameTimeMs = afterRenderTicks - currentTicks;
        if (frameDelay > loopFrameTimeMs) {
            SDL_Delay(frameDelay - loopFrameTimeMs);
        }
    }
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_F1:
                    PhysicsDebugDraw::toggle();
                    break;
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        }
    }
}

void Engine::update() {
    // Refresh mouse position each fixed update to avoid relying solely on motion events
    {
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);
        mouseX = mx;
        mouseY = my;
    }
    if (b2World_IsValid(physicsWorldId)) {
        b2World_Step(physicsWorldId, fixedDeltaTime, 4);

        // Read contact events to detect rocket hitting asteroids (static bodies)
        b2ContactEvents events = b2World_GetContactEvents(physicsWorldId);
        for (int i = 0; i < events.beginCount; ++i) {
            const b2ContactBeginTouchEvent& e = events.beginEvents[i];
            b2BodyId bodyA = b2Shape_GetBody(e.shapeIdA);
            b2BodyId bodyB = b2Shape_GetBody(e.shapeIdB);
            auto* objA = static_cast<GameObject*>(b2Body_GetUserData(bodyA));
            auto* objB = static_cast<GameObject*>(b2Body_GetUserData(bodyB));

            bool aIsPlayer = objA && objA->hasTag("player");
            bool bIsPlayer = objB && objB->hasTag("player");
            bool aIsAsteroid = objA && objA->hasTag("asteroid");
            bool bIsAsteroid = objB && objB->hasTag("asteroid");

            if ((aIsPlayer && bIsAsteroid) || (bIsPlayer && aIsAsteroid)) {
                std::cout << "Rocket hit asteroid" << std::endl;

                // Play explosion sound
                AssetManager::getInstance().playSound("explosion");

                // Freeze and remove the player
                GameObject* playerObj = aIsPlayer ? objA : objB;
                if (playerObj) {
                    // Mark for deletion to remove from game
                    playerObj->markForDeletion();
                    player = nullptr;  // Clear player reference
                    
                    // Trigger game over
                    gameOver = true;
                    gameOverScreen->setScore(score);
                    
                    // Update high score if current score is higher
                    if (score > AssetManager::getInstance().getHighScore()) {
                        AssetManager::getInstance().setHighScore(score);
                        gameOverScreen->setHighScore(score);
                        AssetManager::getInstance().saveHighScoreToXML("assets/config.xml");
                    }
                }

                // Spawn a dedicated explosion emitter GameObject so the player's trail stays intact
                GameObject* hitObj = aIsAsteroid ? objA : objB;
                TransformComponent* hitTransform = hitObj ? hitObj->getComponent<TransformComponent>() : nullptr;
                float x = hitTransform ? hitTransform->getX() : 0.0f;
                float y = hitTransform ? hitTransform->getY() : 0.0f;

                auto explosion = std::make_unique<GameObject>();
                explosion->setTag("explosion");
                auto* t = explosion->addComponent<TransformComponent>();
                t->setPosition(x, y);

                auto* emitter = explosion->addComponent<ParticleEmitterComponent>();
                emitter->setEmitterType(EmitterType::Burst);
                emitter->setBurstCount(explosionConfig.burstCount);
                emitter->setBurstDuration(explosionConfig.burstDuration);
                emitter->setLifetime(explosionConfig.lifetime);
                emitter->setSize(explosionConfig.size);
                emitter->setSpeedMin(explosionConfig.speedMin);
                emitter->setSpeedMax(explosionConfig.speedMax);
                emitter->setSpreadAngle(explosionConfig.spreadAngle);
                emitter->setDirectionOffset(explosionConfig.directionOffset);
                emitter->setPositionOffset(explosionConfig.offsetX, explosionConfig.offsetY);
                emitter->setStartColor(explosionConfig.startColor);
                emitter->setEndColor(explosionConfig.endColor);
                emitter->triggerBurst();

                auto* lifetime = explosion->addComponent<LifetimeComponent>();
                lifetime->setLifetime(explosionConfig.lifetime + 0.3f);

                gameObjects.push_back(std::move(explosion));
            }
        }
    }
    
    // Use fixedDeltaTime to ensure movement independent of render FPS
    for (auto& obj : gameObjects) {
        obj->update(fixedDeltaTime);
    }
    
    // Update particle system
    ParticleSystem::getInstance().update(fixedDeltaTime);
    ParticleSystem::getInstance().update(fixedDeltaTime);
    
    // Update score based on player distance from spawn
    if (player) {
        auto* playerTransform = player->getComponent<TransformComponent>();
        if (playerTransform) {
            float dx = playerTransform->getX() - playerSpawnX;
            float dy = playerTransform->getY() - playerSpawnY;
            float distance = std::sqrt(dx * dx + dy * dy);
            setScore(static_cast<int>(distance)/100);
        }
    }
    
    // Clean up any objects marked for deletion
    cleanupMarkedObjects();
    
    // Process any queued asteroid spawns (happens after update to avoid vector reallocation issues)
    processPendingAsteroids();
}

void Engine::render() {
    // Clear screen with dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    
    // Render all game objects
    for (auto& obj : gameObjects) {
        obj->render();
    }

    // Render particles (after sprites, before debug overlay)
    ParticleSystem::getInstance().render(renderer, &View::getInstance());

    // Physics debug overlay (draw after normal rendering, before present)
    PhysicsDebugDraw::render(renderer, gameObjects);

    // Render score in top left
    if (scoreDisplay && !gameOver) scoreDisplay->render();
    
    // Render game over screen if game is over
    if (gameOver && gameOverScreen) gameOverScreen->render();

    SDL_RenderPresent(renderer);
}

void Engine::quit() {
    running = false;
}

void Engine::clean() {
    // Clear particle system
    ParticleSystem::getInstance().clear();

    // Clean up score display
    if (scoreDisplay) {
        delete scoreDisplay;
        scoreDisplay = nullptr;
    }
    
    // Clean up game over screen
    if (gameOverScreen) {
        delete gameOverScreen;
        gameOverScreen = nullptr;
    }

    // Destroy physics world
    if (b2World_IsValid(physicsWorldId)) {
        b2DestroyWorld(physicsWorldId);
        physicsWorldId = b2_nullWorldId;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    std::cout << "Engine cleaned up" << std::endl;
}

void Engine::setScore(int newScore) {
    score = newScore;
    if (scoreDisplay) scoreDisplay->setScore(score);
    updateAsteroidDifficulty();
}

void Engine::updateAsteroidDifficulty() {
    if (!player) return;
    
    // Calculate spawn density based on score
    // Start at 1.0, increase with score: 1.0 + (score / 1000)
    // So at score 1000, density is 2.0. At score 2000, density is 3.0, etc.
    float baseDensity = 1.0f;
    float difficulty = baseDensity + (score / 1000.0f);
    
    // Get the asteroid spawner component from the player
    auto* spawner = player->getComponent<AsteroidSpawner>();
    if (spawner) {
        spawner->setSpawnDensity(difficulty);
    }
}

void Engine::setTargetFPS(int fps) {
    targetFPS = fps;
}

void Engine::setLogicFPS(int fps) {
    if (fps <= 0) return;
    logicFPS = fps;
    fixedDeltaTime = 1.0f / static_cast<float>(logicFPS);
}

void Engine::removeGameObject(GameObject* obj) {
    if (obj) {
        obj->markForDeletion();
    }
}

void Engine::cleanupMarkedObjects() {
    // Safe erasure pattern: iterate with manual index control
    for (size_t i = 0; i < gameObjects.size(); ) {
        if (gameObjects[i]->isMarkedForDeletion()) {
            // Erase triggers GameObject destructor -> component destructors -> PhysicsBodyComponent cleanup
            gameObjects.erase(gameObjects.begin() + i);
            // Don't increment i since we removed an element
        } else {
            ++i;
        }
    }
}

void Engine::processPendingAsteroids() {
    // Process all queued asteroid spawns
    for (const auto& pending : pendingAsteroids) {
        auto gameObj = std::make_unique<GameObject>();
        gameObj->setTag("asteroid");
        
        // Transform
        auto* transform = gameObj->addComponent<TransformComponent>();
        transform->setPosition(pending.x, pending.y);
        
        // Sprite (set size BEFORE physics to ensure dimensions are available)
        auto* sprite = gameObj->addComponent<SpriteComponent>();
        sprite->setTexture("asteroid");
        sprite->setSizePreserveAspect(pending.size, true);
        
        // Physics (static circular obstacle) - sprite size now available
        auto* physics = gameObj->addComponent<PhysicsBodyComponent>();
        physics->setBodyType(BodyType::Static);
        physics->setShapeType(ShapeType::Circle);
        
        gameObjects.push_back(std::move(gameObj));
    }
    pendingAsteroids.clear();
}

void Engine::updateAsteroidSpawning() {
    // TODO: Implement procedural asteroid spawning based on camera/player position
    // Example logic:
    // - Get player X position
    // - Check if we need to spawn ahead (player X + screen width + buffer)
    // - Randomize Y position, size, spacing based on difficulty
    // - Call spawnAsteroid(x, y, size)
}

void Engine::cleanupOffscreenAsteroids() {
    // TODO: Implement offscreen asteroid cleanup
    // Example logic:
    // - Get player X position
    // - Iterate through gameObjects with tag "asteroid"
    // - Check if asteroid X < player X - screen width - buffer
    // - Mark for deletion via removeGameObject()
}
