// Ball.h

#ifndef BALL_H
#define BALL_H

#include "Entity.h"

class Ball : public Entity {
public:
    Ball();
    void update(SDL_Event* event);
};

#endif