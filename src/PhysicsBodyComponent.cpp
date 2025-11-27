#include "PhysicsBodyComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "Engine.h"
#include <cmath>

PhysicsBodyComponent::~PhysicsBodyComponent() {
    if (b2Body_IsValid(bodyId)) {
        b2DestroyBody(bodyId);
        bodyId = b2_nullBodyId;
    }
}

void PhysicsBodyComponent::init() {
    // Cache commonly used components; defer body creation to update
    transform = owner->getComponent<TransformComponent>();
    sprite = owner->getComponent<SpriteComponent>();
}

void PhysicsBodyComponent::update(float deltaTime) {
    if (!transform) return;

    // Lazy-create physics body once configuration is set
    if (!b2Body_IsValid(bodyId)) {
        b2WorldId worldId = Engine::getInstance().getPhysicsWorld();
        if (!b2World_IsValid(worldId)) return;

        // Create body definition using current config
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = (bodyType == BodyType::Static) ? b2_staticBody :
                       (bodyType == BodyType::Kinematic) ? b2_kinematicBody : b2_dynamicBody;
        bodyDef.position = {transform->getX() / PIXELS_PER_METER, transform->getY() / PIXELS_PER_METER};
        bodyDef.rotation = b2MakeRot(transform->getRotation() * 3.14159f / 180.0f);
        bodyDef.linearDamping = linearDamping;
        bodyDef.angularDamping = angularDamping;
        bodyDef.fixedRotation = fixedRotation;
        bodyDef.userData = owner;

        bodyId = b2CreateBody(worldId, &bodyDef);
        if (!b2Body_IsValid(bodyId)) return;
        b2Body_SetUserData(bodyId, owner);
        b2Body_EnableContactEvents(bodyId, true);

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
        shapeDef.enableContactEvents = true;

        b2ShapeId shapeId;
        if (shapeType == ShapeType::Circle) {
            float radius = std::min(width, height) / 2.0f / PIXELS_PER_METER;
            // Apply uniform collision scaling (use min to keep circle)
            float s = collisionScaleX < collisionScaleY ? collisionScaleX : collisionScaleY;
            if (s > 0.0f) radius *= s;
            b2Circle circle = {{0, 0}, radius};
            shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
        } else {
            const int numVertices = 8; // Box2D polygon vertex limit safety
            b2Vec2 vertices[numVertices];
            float halfWidth = (width / 2.0f / PIXELS_PER_METER) * (collisionScaleX > 0.0f ? collisionScaleX : 1.0f);
            float halfHeight = (height / 2.0f / PIXELS_PER_METER) * (collisionScaleY > 0.0f ? collisionScaleY : 1.0f);
            for (int i = 0; i < numVertices; ++i) {
                float angle = (2.0f * 3.14159f * i) / numVertices;
                vertices[i].x = halfWidth * std::cos(angle);
                vertices[i].y = halfHeight * std::sin(angle);
            }
            b2Hull hull = b2ComputeHull(vertices, numVertices);
            b2Polygon polygon = b2MakePolygon(&hull, 0.0f);
            shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
        }

        b2Shape_SetFriction(shapeId, friction);
        b2Shape_SetRestitution(shapeId, restitution);
    }

    // Sync transform from physics body if body is valid
    if (b2Body_IsValid(bodyId)) {
        b2Vec2 pos = b2Body_GetPosition(bodyId);
        b2Rot rot = b2Body_GetRotation(bodyId);
        float angle = b2Rot_GetAngle(rot) * 180.0f / 3.14159f;
        
        transform->setPosition(pos.x * PIXELS_PER_METER, pos.y * PIXELS_PER_METER);
        transform->setRotation(angle);
    }
}
