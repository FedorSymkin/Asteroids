#ifndef BULLET_H
#define BULLET_H

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "game.h"
#include "actor.h"

class Bullet : public Actor
{
public:
    Bullet(GameEngine *gameEngine, PointF pos_, PointF speed_);
    virtual ~Bullet() {}
    virtual void tick();

protected:
    GLfloat moveCount;
    virtual void renderPos(IGraphicEngine *g, PointF localPos);
};

#endif // BULLET_H
