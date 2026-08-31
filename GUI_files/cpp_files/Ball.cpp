#include "Ball.h"

Ball::Ball() {
    setXPos(0);
    setYPos(0);
    setXVelocity(0);
    setYVelocity(0);
}

void Ball::update(SDL_Event* event) {
    if (event == nullptr) {
        setXPos(getXPos() + getXVelocity());
        setYPos(getYPos() + getYVelocity());
    }
}
