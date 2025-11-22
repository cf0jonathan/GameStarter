#pragma once
#include "Component.h"

class TransformComponent;
class PhysicsBodyComponent;

class RotateToMouseComponent : public Component {
public:
    RotateToMouseComponent() = default;
    
    void init() override;
    void update(float deltaTime) override;
    
    // Backward compatibility: if only rotationSpeed is provided, we derive accel/decel
    void setRotationSpeed(float speed) { maxAngularSpeed = speed; }
    
    void setMaxAngularSpeed(float v) { maxAngularSpeed = v; }
    void setAcceleration(float v) { acceleration = v; }
    void setDeceleration(float v) { deceleration = v; }
    void setSlowRadius(float v) { slowRadius = v; }
    void setDeadZone(float v) { deadZone = v; }
    void setEaseExponent(float v) { easeExponent = v; }
    
private:
    TransformComponent* transform = nullptr;
    PhysicsBodyComponent* physicsBody = nullptr;
    
    // Angular velocity model (degrees)
    float currentAngularSpeed = 0.0f;     // current signed angular speed (deg/sec)
    float maxAngularSpeed = 360.0f;        // top angular speed (deg/sec)
    float acceleration = 720.0f;          // accel rate toward target speed (deg/sec^2)
    float deceleration = 1080.0f;         // decel rate when slowing or flipping (deg/sec^2)
    float slowRadius = 60.0f;             // begin easing when within this many degrees
    float deadZone = 1.0f;                // snap threshold (deg)
    float easeExponent = 0.6f;            // easing curve exponent (0.5-1.0 typical)
};
