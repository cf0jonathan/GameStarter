#pragma once
#include "Component.h"
#include <box2d/box2d.h>

class TransformComponent;
class SpriteComponent;

enum class BodyType {
    Static,    // Immovable (asteroids)
    Dynamic,   // Affected by forces (rocket)
    Kinematic  // Moves but ignores physics
};

enum class ShapeType {
    Circle,
    Ellipse  // Approximated with polygon
};

class PhysicsBodyComponent : public Component {
public:
    PhysicsBodyComponent() = default;
    ~PhysicsBodyComponent();
    
    void init() override;
    void update(float deltaTime) override;
    
    // Configuration (call before init)
    void setBodyType(BodyType type) { bodyType = type; }
    void setShapeType(ShapeType shape) { shapeType = shape; }
    void setDensity(float d) { density = d; }
    void setFriction(float f) { friction = f; }
    void setRestitution(float r) { restitution = r; }
    
    // Runtime access
    b2BodyId getBodyId() const { return bodyId; }
    bool isInitialized() const { return b2Body_IsValid(bodyId); }
    
    // Pixel-to-meter conversion
    static constexpr float PIXELS_PER_METER = 50.0f;
    
private:
    TransformComponent* transform = nullptr;
    SpriteComponent* sprite = nullptr;
    b2BodyId bodyId = b2_nullBodyId;
    
    // Configuration
    BodyType bodyType = BodyType::Dynamic;
    ShapeType shapeType = ShapeType::Circle;
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
};
