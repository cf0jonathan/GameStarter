#include "Game.h"
#include <SDL.h>
#include <box2d/box2d.h>
#include <tinyxml2.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <cmath>

using namespace std::chrono_literals;

int Game::run() {
    if (!loadConfig("assets/config.xml")) {
        std::cerr << "Using defaults (could not load assets/config.xml)\n";
    }
    if (!init()) {
        std::cerr << "Init failed.\n";
        return 1;
    }

    auto start = std::chrono::steady_clock::now();
    float accumulator = 0.0f;
    const float dt = 1.0f / 120.0f; // Smaller timestep for more precise physics (doubled from 60fps to 120fps)
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEMOTION) {
                mouseX_ = e.motion.x;
                mouseY_ = e.motion.y;
                
                // Convert screen coordinates to Box2D world coordinates
                float worldX = (mouseX_ - SCREEN_WIDTH/2) / PIXELS_PER_METER;
                float worldY = (SCREEN_HEIGHT/2 - mouseY_) / PIXELS_PER_METER; // Flip Y axis
                
                // Update the mouse block's physics body position
                if (B2_IS_NON_NULL(mouseBlockId_)) {
                    b2Body_SetTransform(mouseBlockId_, B2_LITERAL(b2Vec2){worldX, worldY}, b2Body_GetRotation(mouseBlockId_));
                }
            }
        }

        accumulator += dt;
        while (accumulator >= dt) {
            stepPhysics(dt);
            accumulator -= dt;
        }

        // Spawn blocks at intervals
        spawnTimer_ += dt;
        if (spawnTimer_ >= spawnInterval_) {
            spawnRandomBlock();
            spawnTimer_ = 0.0f;
        }

        render();

        std::this_thread::sleep_for(10ms);
    }

    shutdown();
    return 0;
}

bool Game::loadConfig(const std::string& path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML: " << path << "\n";
        return false;
    }
    auto* root = doc.RootElement();
    if (!root) return false;
    if (const char* t = root->Attribute("title")) title_ = t;
    root->QueryFloatAttribute("gravityY", &gravityY_);
    float spawnRate;
    root->QueryFloatAttribute("spawnRate", &spawnRate);
    spawnInterval_ = 1.0f / spawnRate; // Convert spawns per second to seconds between spawns
    root->QueryFloatAttribute("boxSize", &boxSize_);
    root->QueryFloatAttribute("mouseBlockSize", &mouseBlockSize_);
    std::cout << "Loaded config: title=\"" << title_ << "\" gravityY=" << gravityY_ 
              << " spawnRate=" << spawnRate << " spawnInterval=" << spawnInterval_ 
              << " boxSize=" << boxSize_ << " mouseBlockSize=" << mouseBlockSize_ << "\n";
    return true;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }
    window_ = SDL_CreateWindow(title_.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window_) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create world with gravity (negative Y pulls downward in Box2D)
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = B2_LITERAL(b2Vec2){0.0f, -gravityY_};
    
    // Enable continuous collision detection to prevent tunneling
    worldDef.enableContinuous = true;
    worldDef.enableSleep = true; // Allow bodies to sleep for better performance
    
    worldId_ = b2CreateWorld(&worldDef);

    // Create boundaries: floor, roof, and side walls (within visible area)
    b2ShapeDef wallShapeDef = b2DefaultShapeDef();
    wallShapeDef.material.friction = 0.6f; // Add friction to walls
    wallShapeDef.material.restitution = 0.2f; // Slight bounciness
    
    // Create floor at bottom of visible screen
    b2BodyDef floorDef = b2DefaultBodyDef();
    floorDef.position = B2_LITERAL(b2Vec2){0.0f, -5.5f}; // Floor near bottom of visible area
    groundId_ = b2CreateBody(worldId_, &floorDef);
    b2Polygon floorBox = b2MakeBox(7.5f, 0.5f); // Fit within screen width
    b2CreatePolygonShape(groundId_, &wallShapeDef, &floorBox);
    
    // Create roof at top of visible screen
    b2BodyDef roofDef = b2DefaultBodyDef();
    roofDef.position = B2_LITERAL(b2Vec2){0.0f, 5.5f}; // Roof near top of visible area
    roofId_ = b2CreateBody(worldId_, &roofDef);
    b2Polygon roofBox = b2MakeBox(7.5f, 0.5f); // Fit within screen width
    b2CreatePolygonShape(roofId_, &wallShapeDef, &roofBox);
    
    // Create left wall
    b2BodyDef leftWallDef = b2DefaultBodyDef();
    leftWallDef.position = B2_LITERAL(b2Vec2){-7.5f, 0.0f}; // Left side within visible area
    leftWallId_ = b2CreateBody(worldId_, &leftWallDef);
    b2Polygon leftWallBox = b2MakeBox(0.5f, 11.0f); // Height to cover visible area
    b2CreatePolygonShape(leftWallId_, &wallShapeDef, &leftWallBox);
    
    // Create right wall
    b2BodyDef rightWallDef = b2DefaultBodyDef();
    rightWallDef.position = B2_LITERAL(b2Vec2){7.5f, 0.0f}; // Right side within visible area
    rightWallId_ = b2CreateBody(worldId_, &rightWallDef);
    b2Polygon rightWallBox = b2MakeBox(0.5f, 11.0f); // Height to cover visible area
    b2CreatePolygonShape(rightWallId_, &wallShapeDef, &rightWallBox);

    // Create mouse-following block (kinematic body for controlled movement)
    b2BodyDef mouseBodyDef = b2DefaultBodyDef();
    mouseBodyDef.type = b2_kinematicBody; // Kinematic so we can control it but it still has collisions
    mouseBodyDef.position = B2_LITERAL(b2Vec2){0.0f, 0.0f}; // Start at world origin
    mouseBodyDef.isBullet = true; // Enable continuous collision detection for mouse block too
    mouseBlockId_ = b2CreateBody(worldId_, &mouseBodyDef);
    
    b2ShapeDef mouseShapeDef = b2DefaultShapeDef();
    mouseShapeDef.density = 1.0f;
    mouseShapeDef.material.friction = 0.4f; // Add friction for better interaction
    mouseShapeDef.material.restitution = 0.3f; // Some bounciness
    b2Polygon mouseBox = b2MakeBox(mouseBlockSize_/2.0f, mouseBlockSize_/2.0f);
    b2CreatePolygonShape(mouseBlockId_, &mouseShapeDef, &mouseBox);

    // Initialize random number generator
    rng_.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    
    // Create block texture once
    int blockSize = static_cast<int>(boxSize_ * PIXELS_PER_METER);
    blockTexture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, 
                                    SDL_TEXTUREACCESS_TARGET, blockSize, blockSize);
    if (!blockTexture_) {
        std::cerr << "Failed to create block texture: " << SDL_GetError() << "\n";
        return false;
    }
    
    // Draw the block pattern onto the texture
    SDL_SetRenderTarget(renderer_, blockTexture_);
    SDL_SetRenderDrawColor(renderer_, 255, 100, 100, 255); // Red
    SDL_RenderClear(renderer_);
    SDL_SetRenderTarget(renderer_, nullptr);
    
    std::cout << "Init complete. Falling blocks will spawn randomly.\n";
    return true;
}

void Game::shutdown() {
    if (B2_IS_NON_NULL(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
        mouseBlockId_ = b2_nullBodyId; // Reset mouse block ID
        groundId_ = b2_nullBodyId; // Reset ground ID
        leftWallId_ = b2_nullBodyId; // Reset left wall ID
        rightWallId_ = b2_nullBodyId; // Reset right wall ID
        roofId_ = b2_nullBodyId; // Reset roof ID
    }
    if (blockTexture_) { SDL_DestroyTexture(blockTexture_); blockTexture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    SDL_Quit();
}

void Game::stepPhysics(float dt) {
    // Clamp velocities to prevent objects from moving too fast (prevents tunneling)
    const float maxVelocity = 20.0f; // Maximum velocity in m/s
    
    for (const auto& block : fallingBlocks_) {
        if (B2_IS_NON_NULL(block.bodyId)) {
            b2Vec2 velocity = b2Body_GetLinearVelocity(block.bodyId);
            float speed = b2Length(velocity);
            
            if (speed > maxVelocity) {
                // Scale velocity down to maximum allowed speed
                b2Vec2 clampedVelocity = b2MulSV(maxVelocity / speed, velocity);
                b2Body_SetLinearVelocity(block.bodyId, clampedVelocity);
            }
        }
    }
    
    // Use much more sub-steps for better collision detection (prevents tunneling)
    // Higher sub-steps = more accurate collision detection for fast objects
    int subSteps = 24; // Significantly increased for very precise collision detection
    b2World_Step(worldId_, dt, subSteps);
}

void Game::render() {
    // Clear screen with dark blue background
    SDL_SetRenderDrawColor(renderer_, 20, 20, 30, 255);
    SDL_RenderClear(renderer_);

    // Simple coordinate conversion: Box2D world to screen pixels
    // Screen center is at (400, 300), Box2D origin is at (0, 0)
    auto worldToScreen = [this](b2Vec2 worldPos) -> SDL_Point {
        SDL_Point screenPos;
        screenPos.x = static_cast<int>(SCREEN_WIDTH/2 + worldPos.x * PIXELS_PER_METER);
        screenPos.y = static_cast<int>(SCREEN_HEIGHT/2 - worldPos.y * PIXELS_PER_METER); // Flip Y axis
        return screenPos;
    };

    // Draw boundaries (floor, roof, and walls) - within visible area
    SDL_SetRenderDrawColor(renderer_, 100, 100, 100, 255); // Gray color for walls
    
    // Draw floor at bottom
    SDL_Rect floorRect;
    floorRect.x = SCREEN_WIDTH/2 - static_cast<int>(7.5f * PIXELS_PER_METER);
    floorRect.y = SCREEN_HEIGHT/2 + static_cast<int>(5.5f * PIXELS_PER_METER) - static_cast<int>(0.5f * PIXELS_PER_METER);
    floorRect.w = static_cast<int>(15.0f * PIXELS_PER_METER);
    floorRect.h = static_cast<int>(1.0f * PIXELS_PER_METER);
    SDL_RenderFillRect(renderer_, &floorRect);
    
    // Draw roof at top
    SDL_Rect roofRect;
    roofRect.x = SCREEN_WIDTH/2 - static_cast<int>(7.5f * PIXELS_PER_METER);
    roofRect.y = SCREEN_HEIGHT/2 - static_cast<int>(5.5f * PIXELS_PER_METER) - static_cast<int>(0.5f * PIXELS_PER_METER);
    roofRect.w = static_cast<int>(15.0f * PIXELS_PER_METER);
    roofRect.h = static_cast<int>(1.0f * PIXELS_PER_METER);
    SDL_RenderFillRect(renderer_, &roofRect);
    
    // Draw left wall
    SDL_Rect leftWallRect;
    leftWallRect.x = SCREEN_WIDTH/2 - static_cast<int>(7.5f * PIXELS_PER_METER) - static_cast<int>(0.5f * PIXELS_PER_METER);
    leftWallRect.y = SCREEN_HEIGHT/2 - static_cast<int>(11.0f * PIXELS_PER_METER);
    leftWallRect.w = static_cast<int>(1.0f * PIXELS_PER_METER);
    leftWallRect.h = static_cast<int>(22.0f * PIXELS_PER_METER);
    SDL_RenderFillRect(renderer_, &leftWallRect);
    
    // Draw right wall
    SDL_Rect rightWallRect;
    rightWallRect.x = SCREEN_WIDTH/2 + static_cast<int>(7.5f * PIXELS_PER_METER) - static_cast<int>(0.5f * PIXELS_PER_METER);
    rightWallRect.y = SCREEN_HEIGHT/2 - static_cast<int>(11.0f * PIXELS_PER_METER);
    rightWallRect.w = static_cast<int>(1.0f * PIXELS_PER_METER);
    rightWallRect.h = static_cast<int>(22.0f * PIXELS_PER_METER);
    SDL_RenderFillRect(renderer_, &rightWallRect);

    // Draw all falling blocks with rotation using color modulation
    for (const auto& block : fallingBlocks_) {
        if (B2_IS_NON_NULL(block.bodyId)) {
            b2Vec2 blockPos = b2Body_GetPosition(block.bodyId);
            b2Transform transform = b2Body_GetTransform(block.bodyId);
            
            // Convert Box2D transform to screen coordinates
            int centerX = SCREEN_WIDTH/2 + static_cast<int>(blockPos.x * PIXELS_PER_METER);
            int centerY = SCREEN_HEIGHT/2 - static_cast<int>(blockPos.y * PIXELS_PER_METER);
            
            // Get rotation angle from transform (convert to degrees for SDL)
            float angle = atan2(transform.q.s, transform.q.c) * 180.0f / 3.14159f;
            
            // Set color modulation for this block
            SDL_SetTextureColorMod(blockTexture_, block.color.r, block.color.g, block.color.b);
            
            // Draw the rotated texture with color modulation
            int blockSize = static_cast<int>(boxSize_ * PIXELS_PER_METER);
            SDL_Rect destRect = {centerX - blockSize/2, centerY - blockSize/2, blockSize, blockSize};
            SDL_RenderCopyEx(renderer_, blockTexture_, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
        }
    }

    // Draw mouse-following block (with physics)
    if (B2_IS_NON_NULL(mouseBlockId_)) {
        b2Vec2 mouseBlockPos = b2Body_GetPosition(mouseBlockId_);
        b2Transform mouseTransform = b2Body_GetTransform(mouseBlockId_);
        
        // Convert Box2D position to screen coordinates
        int centerX = SCREEN_WIDTH/2 + static_cast<int>(mouseBlockPos.x * PIXELS_PER_METER);
        int centerY = SCREEN_HEIGHT/2 - static_cast<int>(mouseBlockPos.y * PIXELS_PER_METER);
        
        // Get rotation angle from transform (convert to degrees for SDL)
        float angle = atan2(mouseTransform.q.s, mouseTransform.q.c) * 180.0f / 3.14159f;
        
        // Set yellow color for mouse block
        SDL_SetTextureColorMod(blockTexture_, 255, 255, 0);
        
        // Draw the rotated mouse block
        int mouseBlockPixelSize = static_cast<int>(mouseBlockSize_ * PIXELS_PER_METER);
        SDL_Rect destRect = {centerX - mouseBlockPixelSize/2, centerY - mouseBlockPixelSize/2, mouseBlockPixelSize, mouseBlockPixelSize};
        SDL_RenderCopyEx(renderer_, blockTexture_, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
    }

    // Present the rendered frame
    SDL_RenderPresent(renderer_);
}

void Game::spawnRandomBlock() {
    // Random position within the walls (between -6.5 and 6.5 X, between -4.5 and 4.5 Y)
    std::uniform_real_distribution<float> xDist(-6.5f, 6.5f); // Inside left and right walls
    std::uniform_real_distribution<float> yDist(-4.5f, 4.5f); // Inside floor and roof
    std::uniform_int_distribution<int> colorDist(0, 255);
    
    float x = xDist(rng_);
    float y = yDist(rng_);

    // Generate random color
    SDL_Color blockColor = {
        static_cast<Uint8>(colorDist(rng_)), // Red
        static_cast<Uint8>(colorDist(rng_)), // Green  
        static_cast<Uint8>(colorDist(rng_)), // Blue
        255 // Alpha (fully opaque)
    };
    
    // Create falling block at top of screen
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = B2_LITERAL(b2Vec2){x, y}; // Start at top
    
    // Enable bullet physics for continuous collision detection
    bodyDef.enableSleep = true; // Allow sleeping for performance
    bodyDef.isBullet = true; // Enable continuous collision detection for this body
    
    b2BodyId blockId = b2CreateBody(worldId_, &bodyDef);
    
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.density = 1.0f;
    dynamicShapeDef.material.restitution = 0.3f; // Add some bounciness
    dynamicShapeDef.material.friction = 0.4f; // Add friction for better stacking
    
    // Enable continuous collision detection for fast-moving objects
    dynamicShapeDef.enableHitEvents = true; // Enable collision events
    
    b2Polygon dynamicBox = b2MakeBox(boxSize_/2.0f, boxSize_/2.0f); // b2MakeBox uses half-width/half-height
    b2CreatePolygonShape(blockId, &dynamicShapeDef, &dynamicBox);
    
    // Give each block random initial velocity and direction
    std::uniform_real_distribution<float> velocityXDist(-5.0f, 5.0f); // Random horizontal velocity
    std::uniform_real_distribution<float> velocityYDist(-2.0f, 2.0f); // Random vertical velocity
    std::uniform_real_distribution<float> angularVelDist(-3.14f, 3.14f); // Random rotation velocity
    
    float randomVelX = velocityXDist(rng_);
    float randomVelY = velocityYDist(rng_);
    float randomAngularVel = angularVelDist(rng_);
    
    // Apply the random velocity to the block
    b2Body_SetLinearVelocity(blockId, B2_LITERAL(b2Vec2){randomVelX, randomVelY});
    b2Body_SetAngularVelocity(blockId, randomAngularVel);
    
    // Store block with its color
    fallingBlocks_.push_back({blockId, blockColor});
}
