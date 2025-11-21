#include "PhysicsBodyComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "Engine.h"
#include <cmath>

PhysicsBodyComponent::~PhysicsBodyComponent() {
    if (b2Body_IsValid(bodyId)) {
        b2DestroyBody(bodyId);
    }
}

void PhysicsBodyComponent::init() {
    transform = owner->getComponent<TransformComponent>();
    sprite = owner->getComponent<SpriteComponent>();
    
    if (!transform) return;
    
    b2WorldId worldId = Engine::getInstance().getPhysicsWorld();
    if (!b2World_IsValid(worldId)) return;
    
    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = (bodyType == BodyType::Static) ? b2_staticBody :
                   (bodyType == BodyType::Kinematic) ? b2_kinematicBody : b2_dynamicBody;
    bodyDef.position = {transform->getX() / PIXELS_PER_METER, transform->getY() / PIXELS_PER_METER};
    bodyDef.rotation = b2MakeRot(transform->getRotation() * 3.14159f / 180.0f);
    bodyDef.linearDamping = linearDamping;
    bodyDef.angularDamping = angularDamping;
    bodyDef.fixedRotation = fixedRotation;
    
    bodyId = b2CreateBody(worldId, &bodyDef);
    
    // Auto-size shape from sprite dimensions
    float width = 50.0f;
    float height = 50.0f;
    if (sprite) {
        width = sprite->getWidth();
        height = sprite->getHeight();
    }
    
    // Create shape
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    
    b2ShapeId shapeId;
    if (shapeType == ShapeType::Circle) {
        // Circle fits inside bounding box
        float radius = std::max(width, height) / 2.0f / PIXELS_PER_METER;
        b2Circle circle = {{0, 0}, radius};
        shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
    } else {
        // Ellipse approximation with 12-vertex polygon
        const int numVertices = 12;
        b2Vec2 vertices[numVertices];
        float halfWidth = width / 2.0f / PIXELS_PER_METER;
        float halfHeight = height / 2.0f / PIXELS_PER_METER;
        
        for (int i = 0; i < numVertices; ++i) {
            float angle = (2.0f * 3.14159f * i) / numVertices;
            vertices[i].x = halfWidth * std::cos(angle);
            vertices[i].y = halfHeight * std::sin(angle);
        }
        
        b2Hull hull = b2ComputeHull(vertices, numVertices);
        b2Polygon polygon = b2MakePolygon(&hull, 0.0f);
        shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }
    
    // Set friction and restitution on the created shape
    b2Shape_SetFriction(shapeId, friction);
    b2Shape_SetRestitution(shapeId, restitution);
}

void PhysicsBodyComponent::update(float deltaTime) {
    if (!b2Body_IsValid(bodyId) || !transform) return;
    
    // Sync transform from physics body
    b2Vec2 pos = b2Body_GetPosition(bodyId);
    b2Rot rot = b2Body_GetRotation(bodyId);
    float angle = b2Rot_GetAngle(rot) * 180.0f / 3.14159f;
    
    transform->setPosition(pos.x * PIXELS_PER_METER, pos.y * PIXELS_PER_METER);
    transform->setRotation(angle);
}
