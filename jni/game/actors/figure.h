#ifndef FIGURE_H
#define FIGURE_H

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "game.h"
#include "actor.h"

class Figure : public Actor
{  
public:
    Figure(GameEngine *gameEngine, Points selfVert);
    virtual ~Figure() {}
    void crush(PointF base, GLfloat lineAngle);

    virtual void tick();

    bool    collision();
    bool    collision(Figure *other);
    bool    properVertices();

    unsigned int verticesCount() {return selfVertices.size();}
    void    setSelfVertices(Points &points);

    /*
     *sceneVertex, sceneVertices
     *используется при взаимодействии с другим объектом,
     *находящимся на позиции relativePoint. Необходимо для
     *правильного взаимодействия объектов в условиях замкнутого мира
    */
    PointF  sceneVertex(PointF relativePoint, int i);
    Points  sceneVertices(PointF relativePoint);

    GLfloat mass();
    GLfloat perimeter();

    bool    immediatelyKill;
    GLfloat size;

protected:
    Points selfVertices;

    virtual void renderPos(IGraphicEngine *g, PointF localPos);
    virtual void debugRender(IGraphicEngine *g);
    virtual void onCrush() {} 

    bool updateSpeed(Figure *other, GLfloat corrAngle);
    bool collisionProcessing(Figure *other, GLfloat corrAngle);
    bool doHit(Figure *hitter, GLfloat correction);

    PointF oldPos, oldSpeed;
    void saveParameters()       {oldPos = pos; oldSpeed = speed;}
    void restoreParameters()    {pos = oldPos; speed = oldSpeed;}
    void restorePos()           {pos = oldPos;}
    void restep(GLfloat x = 1)  {restorePos(); step(x);}

    Points toTriangles(PointF relativePoint);
};

#endif // ACTOR_H
