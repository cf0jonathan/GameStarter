#pragma once

namespace tinyxml2 {
    class XMLElement;
}

// Abstract base class for all game objects
class Object {
protected:
    float x{};
    float y{};
    float angle{};

public:
    virtual ~Object() = default;

    virtual void update() = 0;
    virtual void draw() = 0;

    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }

    void setPosition(float newX, float newY, float newAngle) {
        x = newX;
        y = newY;
        angle = newAngle;
    }
};