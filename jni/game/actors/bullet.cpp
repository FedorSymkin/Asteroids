#include "game.h"
#include "actor.h"
#include "bullet.h"
#include "explosion.h"



Bullet::Bullet(GameEngine *gameEngine, PointF pos_, PointF speed_):
    Actor::Actor(gameEngine),
    moveCount(0)
{
    type |= TYPE_BULLET;

    pos = pos_;
    speed = speed_;
}

void Bullet::tick()
{
    PointF oldPos = pos;
    step();
    PointF newPos = scenePos(oldPos);

    for (unsigned int i = 0; i < game->actors.size(); i++)
    {
        int ftype = game->actors[i]->getType();
        if ((ftype & TYPE_FIGURE) && (!(ftype & TYPE_SPACESHIP)))
        {
            Figure *f = (Figure *)game->actors[i];
            Points points = f->sceneVertices(oldPos);
            for (unsigned int i = 0, j = points.size() - 1; i < points.size(); j = i++)
            {
                if (intersectSegments(oldPos, newPos, points[i], points[j]))
                {
                    f->crush(newPos, ang(speed));
                    willRemoved_ = true;
                    game->addActor(new Explosion(game, newPos));
                    break;
                }
            }
        }
    }

    moveCount += speed.abs();
    if (moveCount >= BULLET_MAX_MOVE) willRemoved_ = true;
}

void Bullet::renderPos(IGraphicEngine *g, PointF localPos)
{
    g->drawCircle(localPos, BULLET_RADIUS, BULLET_COLOR, false);
}

