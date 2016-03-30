#include "game.h"
#include "actor.h"
#include "explosion.h"


Explosion::Explosion(GameEngine *gameEngine, PointF pos_) :
    Actor::Actor(gameEngine),
    radius(0)
{
    type |= TYPE_EXPLOSION;

    pos = pos_;
}

void Explosion::renderPos(IGraphicEngine *g, PointF localPos)
{
    g->drawCircle(localPos, radius, EXPLOSION_COLOR, true);
}

void Explosion::tick()
{
    radius += EXPLOSION_SPEED;
    if (radius >= EXPLOSION_RADIUS)
    {
        this->willRemoved_ = true;
    }
}
