#ifndef GAME_H_
#define GAME_H_

#include "graphicsEngine.h"
#include "utils.h"
#include "geometry.h"
#include "config.h"

#include "actor.h"
#include "spaceship.h"

class GameEngine
{
public:
    GameEngine(int w, int h);
    void init();
    void deinit();
    void tick();
    void render(IGraphicEngine *graphics);

    void touchEvent(PointF screenPoint, bool down);
    void keyEvent(int key, bool down);

    const int screenWigth, screenHeight;
    const GLfloat aspectRatio;
    const GLfloat boundX, boundY;
    bool  inBounds(PointF p);

    int avgTickTime() {return avgtt;}
    int maxTickTime() {return maxtt;}

private:
    Timer tickTmr;
    Timer newLevelTmr;
    Timer newPlayerTmr;
    Timer debugTmr;
    Timer touchTmr;

    vector<Actor*>  actors; //do not actors.push_back(). Use addActor() instead.
    vector<Actor*>  actorsToAdd;
    Spaceship*      player;
    int             level;
    int             lives;

    void        initLevel(int level);
    void        clearLevel();
    void        defeat();
    void        victory();

    void        addActor(Actor* actor);
    Spaceship*  makePlayer();
    Figure*     makeFigure();

    PointF screenToGame(PointF p);

    int     elapsedMs;
    char    touchState;
    PointF  touchPoint;
    PointF  playerTouchPoint;

    //debug statistics
    int     avgtt;
    int     maxtt;
    int     ticksCntr;
    int     avgCntr;

    friend class Actor;
    friend class Figure;
    friend class Spaceship;
    friend class Bullet;
    friend class Explosion;
};

#endif
