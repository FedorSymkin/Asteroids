#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "game.h"
#include "actor.h"
#include "figure.h"

class Spaceship : public Figure
{
public:
    Spaceship(GameEngine *gameEngine);
    virtual ~Spaceship() {}

    void    setAction(char action, GLfloat angle, GLfloat param);
    void    die(PointF relativePoint);

    bool    isAlive()        {return alive;}
    bool    isAccelerated()  {return acceleration;}
    GLfloat directionAngle() {return dirAngle;}

protected:
    GLfloat     dirAngle;
    GLfloat     needAngle;
    bool        alive;
    char        action;
    GLfloat     actionParam;
    Timer       moveTimer;
    bool        acceleration;


    PointF      shotPoint() {return sceneVertex(this->pos, 0);}
    void        shot();
    void        setDirectionAngle(GLfloat angle);
    void        updateSelfVertices();
    void        applyAction();

    virtual void tick();
    virtual void renderPos(IGraphicEngine *g, PointF localPos);
    virtual void onCrush();
};

#endif // SPACESHIP_H
