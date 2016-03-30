#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "game.h"
#include "actor.h"

class Explosion : public Actor
{
public:
    Explosion(GameEngine *gameEngine, PointF pos_);
    virtual ~Explosion() {}
    virtual void tick();

protected:
    GLfloat radius;
    virtual void renderPos(IGraphicEngine *g, PointF localPos);
};
#endif // EXPLOSION_H
