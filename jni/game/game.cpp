#include "game.h"
#include "actor.h"
#include "figure.h"
#include "spaceship.h"
#include "bullet.h"
#include "explosion.h"

#include <sys/time.h>
#include <time.h>


enum
{
    TOUCH_STATE_NONE = 0,
    TOUCH_STATE_SHOT_OR_ACCELERATE,
    TOUCH_STATE_MOVE
};


GameEngine::GameEngine(int w, int h) :
    screenWigth(w),
    screenHeight(h),
    aspectRatio((GLfloat)screenHeight / GLfloat(w)),
    boundX( (screenHeight >= w) ? 1 : aspectRatio ),
    boundY( (w >= screenHeight) ? 1 : aspectRatio )
{
	logi("GameEngine()");
    logi("screenWigth = %d", screenWigth);
    logi("screenHeight = %d", screenHeight);
    logi("aspectRatio = %f", aspectRatio);
    logi("boundX = %f", boundX);
    logi("boundY = %f", boundY);

    player = 0;
    level = INITIAL_LEVEL;
    lives = INITIAL_LIVES;
    elapsedMs = 0;
    avgtt = 0;
    maxtt = 0;
    ticksCntr = 0;
    avgCntr = 0;
    touchState = TOUCH_STATE_NONE;
}

void GameEngine::init()
{
    logi("GameEngine::init()");

    randomize();
    initLevel(level);
    tickTmr.start();
    debugTmr.start();
}

void GameEngine::deinit()
{
    logi("GameEngine::deinit()");
    clearLevel();
}

void GameEngine::initLevel(int level)
{
    logi("init level %d", level);

    clearLevel();


    player = makePlayer();
    addActor(player);
    for (int i = 0; i < level + 3; i++)
    {
        Figure *f = makeFigure();
        addActor(f);
    }
}

void GameEngine::clearLevel()
{
    logi("clear level");

    while (actors.size())
    {
        delete actors[0];
        actors.erase(actors.begin());
    }

    while (actorsToAdd.size())
    {
        delete actorsToAdd[0];
        actorsToAdd.erase(actorsToAdd.begin());
    }

    player = 0;
}


Spaceship *GameEngine::makePlayer()
{
    logi("make player");

    Spaceship *s = 0;

    PointF pos;
    while (true)
    {
        s = new Spaceship(this);
        s->pos = pos;

        if (s->collision())
        {
            delete s;
            pos.x = frand(-boundX, boundX);
            pos.y = frand(-boundY, boundY);
            continue;
        }
        break;
    }
    return s;
}

Figure *GameEngine::makeFigure()
{
    logi("make figure");

    Figure *f = 0;
    while (true)
    {
        //make random vertices and position==============================
        Points points;
        PointF pos;

        int vertCount = irand(FIGURE_MIN_VERTICES_COUNT, FIGURE_MAX_VERTICES_COUNT);
        GLfloat angle = 0;

        for (int i = 0; i < vertCount; i++)
        {
            GLfloat minAngle = dr(360 / ((GLfloat)vertCount * 2));
            GLfloat maxAngle = dr(360 / (GLfloat)vertCount);
            angle += frand(minAngle, maxAngle);
            GLfloat r = frand((GLfloat)FIGURE_MIN_SIZE/2, (GLfloat)FIGURE_MAX_SIZE/2);

            PointF p(r * cos(angle), r * sin(angle));
            points.push_back(p);
        }
        pos = PointF( frand(-boundX, boundX), frand(-boundY, boundY) );

        f = new Figure(this, points);
        f->pos = pos;


        //check figure======================================================
        bool nearPlayer = false;
        if (player)
        {
            nearPlayer = (RangeF(f->pos, player->scenePos(f->pos)) < PLAYER_SAFE_ZONE);
        }

        if ((f->collision()) || (nearPlayer) || (!f->properVertices()))
        {
            delete f;
            continue;
        }

        f->speed = PointF(
                            frand(-MAX_INITIAL_FIGURE_SPEED, MAX_INITIAL_FIGURE_SPEED),
                            frand(-MAX_INITIAL_FIGURE_SPEED, MAX_INITIAL_FIGURE_SPEED)
                            );

        break;
    }

    return f;
}


void GameEngine::tick()
{
    elapsedMs += tickTmr.get();
    if (elapsedMs > 0) tickTmr.clear();

    if (elapsedMs >= TICK_TIME)
    {
        elapsedMs = 0;

        //delete some actors===================================================
        for (std::vector<Actor*>::iterator i = actors.begin(); i != actors.end(); )
        {
            if ((*i)->willRemoved())
            {
                delete (*i);
                i = actors.erase(i);
            } else i++;
        }

        //add new actors===================================================
        for (unsigned int i = 0; i < actorsToAdd.size(); i++)
        {
            actors.push_back(actorsToAdd[i]);
        }
        actorsToAdd.clear();


        //tick actors===================================================
        for (unsigned int i = 0; i < actors.size(); i++)
        {
            actors[i]->tick();
        }

        //check for new level===================================================
        if (newLevelTmr.get() >= STD_PAUSE)
        {
            if (level < MAX_LEVEL) level++;
            initLevel(level);
            newLevelTmr.stop();
        }

        //check for Victory===================================================
        if (actors.size() == 1)
        {
            if (actors[0] == player)
            {
                victory();
            }
        }

        //check for new player===================================================
        if ((newPlayerTmr.get() >= STD_PAUSE))
        {
            player = makePlayer();
            addActor(player);
            newPlayerTmr.stop();
        }


        //check player commands===================================================
        if (touchTmr.get() > ACCELERATE_TOUT)
        {
            if (player)
            {
                player->setAction('a', ang(touchPoint - player->pos), 0);
            }
        }


        //update debug statistics===================================================
        ticksCntr++;
        int t = tickTmr.get();
        avgCntr += t;
        if (t > maxtt) maxtt = t;

        if (debugTmr.get() >= 1000)
        {
            avgtt = avgCntr / ticksCntr;
            avgCntr = 0;
            ticksCntr = 0;
            debugTmr.clear();
        }
    }
}

void GameEngine::victory()
{
    if (!newLevelTmr.active())
    {
        logi("victory");
        newLevelTmr.start();
    }
}

void GameEngine::defeat()
{
    logi("defeat");

    player = 0;
    touchState = TOUCH_STATE_NONE;
    touchTmr.stop();

    lives--;
    if (lives > 0)
    {
        if (!newPlayerTmr.active()) newPlayerTmr.start();
    }
}


void drawLivePictogram(IGraphicEngine *graphics, PointF p)
{
    GLfloat a;
    GLfloat r = LIVE_PICT_SIZE;
    Points points;

    a = dr(90) ; points.push_back(p + PointF( r * cos(a), r * sin(a)) * 1.0f);
    a = dr(225); points.push_back(p + PointF( r * cos(a), r * sin(a)) * 0.7f);
    a = dr(270); points.push_back(p + PointF( r * cos(a), r * sin(a)) * 0.15f);
    a = dr(315); points.push_back(p + PointF( r * cos(a), r * sin(a)) * 0.7f);

    graphics->drawPolygon(points, LIVE_PICT_COLOR);
}

void GameEngine::render(IGraphicEngine *graphics)
{
    graphics->clear(BACKGROUND_COLOR);

    for (unsigned int i = 0; i < actors.size(); i++)
        actors[i]->render(graphics);

    for (int i = 0; i < lives; i++)
        drawLivePictogram(graphics, PointF(-boundX + 0.1 + i * 0.15, boundY - 0.1));
}

void GameEngine::touchEvent(PointF screenPoint, bool down)
{
    PointF gamePoint = screenToGame(screenPoint);
    //logi("touchEvent on (%f, %f) down = %d", gamePoint.x, gamePoint.y, down);
    touchPoint = gamePoint;


    if (player)
    {
        switch (touchState)
        {
        case TOUCH_STATE_NONE:
            if (down)
            {
                PointF r = gamePoint - player->pos;
                bool touchOnPlayer = r.abs() <= player->size;

                if (touchOnPlayer)
                {
                    playerTouchPoint = gamePoint;
                    touchState = TOUCH_STATE_MOVE;
                }
                else
                {
                    player->setAction('r', ang(r), 0); //rotate
                    touchState = TOUCH_STATE_SHOT_OR_ACCELERATE;
                    touchTmr.start();
                }
            }
            else //if (up)
            {
            }
            break;

        case TOUCH_STATE_SHOT_OR_ACCELERATE:
            if (down)
            {
            }
            else //if (up)
            {
                //shot
                if (touchTmr.get() <= ACCELERATE_TOUT)
                {
                    PointF r = gamePoint - player->pos;
                    player->setAction('s', ang(r), 0);
                    touchState = TOUCH_STATE_NONE;
                }
                //accelerate
                else
                {
                    player->setAction('f', 0, 0); //free action
                    touchState = TOUCH_STATE_NONE;
                }
                touchTmr.stop();
            }
            break;

        case TOUCH_STATE_MOVE:
            if (down)
            {

            }
            else //if (up)
            {
                PointF r = gamePoint - playerTouchPoint;
                player->setAction('m', ang(r), r.abs());
                touchState = TOUCH_STATE_NONE;
            }
            break;
        }
    } else touchState = TOUCH_STATE_NONE;
}

void GameEngine::keyEvent(int key, bool down)
{
    logi("touchEvent key = %d down = %d", key, down);

    //empty
}

PointF GameEngine::screenToGame(PointF p)
{
    return PointF (
                    (p.x / screenWigth) * 2 - 1,
                     -((p.y / screenHeight) * 2 - 1) * aspectRatio
                   );
}

bool GameEngine::inBounds(PointF p)
{
    return ((p.x >= -boundX) && (p.x <= boundX) &&
            (p.y >= -boundY) && (p.y <= boundY));

}

void GameEngine::addActor(Actor *actor)
{
    actorsToAdd.push_back(actor);
}

