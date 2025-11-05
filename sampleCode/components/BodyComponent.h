#pragma once
#include "Component.h"

class BodyComponent : public Component {
public:
    BodyComponent(GameObject& owner, double x, double y)
        : Component(owner), _x(x), _y(y) {}

    double& x() { return _x; }
    double& y() { return _y; }

    void draw () override {
        this->parent();
    }
    void update () override {}


private:
    double _x, _y;
};
