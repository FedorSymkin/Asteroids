#include "game.h"
#include "actor.h"


Actor::Actor(GameEngine *gameEngine) :
    game(gameEngine),
    willRemoved_(false)
{
    type = 0;
}


void Actor::render(IGraphicEngine *g)
{
    renderPos(g, pos);

    renderPos(g, pos + PointF( 0,                 game->boundY*2));
    renderPos(g, pos + PointF( game->boundX*2,    game->boundY*2));
    renderPos(g, pos + PointF( game->boundX*2,    0));
    renderPos(g, pos + PointF( game->boundX*2,    -game->boundY*2));
    renderPos(g, pos + PointF( 0,                 -game->boundY*2));
    renderPos(g, pos + PointF(-game->boundX*2,    -game->boundY*2));
    renderPos(g, pos + PointF(-game->boundX*2,    0));
    renderPos(g, pos + PointF(-game->boundX*2,    game->boundY*2));

#ifdef ASTERIODS_DEBUG
    debugRender(g);
#endif
}

void Actor::step(GLfloat x)
{
    pos += speed*x;

    if      (pos.x >  game->boundX) pos.x -= game->boundX*2;
    else if (pos.x < -game->boundX) pos.x += game->boundX*2;

    if      (pos.y >  game->boundY) pos.y -= game->boundY*2;
    else if (pos.y < -game->boundY) pos.y += game->boundY*2;
}

/*
 *scenePos используется при взаимодействии с другим объектом,
 *находящимся на позиции relativePoint. Необходимо для
 *правильного взаимодействия объектов в условиях замкнутого мира
*/
PointF Actor::scenePos(PointF relativePoint)
{
    return pos + getCyclicOffset(relativePoint);
}

PointF Actor::getCyclicOffset(PointF relativePoint)
{
    PointF offset;

    GLfloat dx = (relativePoint.x - pos.x);
    GLfloat dy = (relativePoint.y - pos.y);

    if      (dx > game->boundX)   offset.x = game->boundX*2;
    else if (dx < -game->boundX)  offset.x = -game->boundX*2;

    if      (dy > game->boundY)   offset.y = game->boundY*2;
    else if (dy < -game->boundY)  offset.y = -game->boundY*2;

    return offset;
}

