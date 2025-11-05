#pragma once
#include "Component.h"
#include "BodyComponent.h"


class SlideComponent : public Component {
public:
    SlideComponent(GameObject& parent) : Component(parent) {
        speed = 5;
        leftx = 0;
        rightx = Engine::width;
    }

    SlideComponent(GameObject& parent, double speed) 
        : Component(parent), speed(speed){
        leftx = 0;
        rightx = Engine::width;
    }
    SlideComponent(GameObject& parent, double speed, int leftx, int rightx) 
        : Component(parent), speed(speed), leftx(leftx), rightx(rightx){}
    
    void update() override {}

    void draw() override {
        auto body = parent().get<BodyComponent>();
        //body->x()+=100*Engine::deltatime;
        if (body->x() > rightx) {
            goingRight = false;
            body->x() = rightx;
        }
        if (body->x() < leftx ) {
            goingRight = true;
            body->x() = leftx;
        }


        if (goingRight) {
            body->x() += speed;
        } else {
            body->x() -= speed;
        }
    }



private:
    double speed;
    bool goingRight{ true };
    int leftx;
    int rightx;
};