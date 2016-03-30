#ifndef ACTOR_H
#define ACTOR_H

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "game.h"

#define TYPE_FIGURE     (int)0x01
#define TYPE_SPACESHIP  (int)0x02
#define TYPE_BULLET     (int)0x04
#define TYPE_EXPLOSION  (int)0x08

class GameEngine;
class Actor
{
public:
    Actor(GameEngine *gameEngine);
    virtual ~Actor() {}
    bool willRemoved() {return willRemoved_;}

    GameEngine* const game;
    PointF pos;
    PointF speed;

    /*
     *scenePos используется при взаимодействии с другим объектом,
     *находящимся на позиции relativePoint. Необходимо для
     *правильного взаимодействия объектов в условиях замкнутого мира
    */
    PointF scenePos(PointF relativePoint);

    virtual void tick() = 0;
    void render(IGraphicEngine *g);

    int getType() {return type;}

protected:
    virtual void renderPos(IGraphicEngine *g, PointF localPos) = 0;
    virtual void debugRender(IGraphicEngine *g) {}
    virtual void step(GLfloat x = 1);

    PointF getCyclicOffset(PointF relativePoint);

    bool willRemoved_;
    int type;
};

#endif // ACTOR_H
