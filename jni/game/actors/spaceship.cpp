#include "game.h"
#include "actor.h"
#include "figure.h"
#include "spaceship.h"
#include "bullet.h"
#include "explosion.h"


Spaceship::Spaceship(GameEngine *gameEngine) :
    Figure::Figure(gameEngine, Points())
{
	logi("Spaceship()");
    type |= TYPE_SPACESHIP;

    setDirectionAngle(dr(90));
    updateSelfVertices();

    needAngle = dirAngle;
    alive = true;
    action = 0;
    actionParam = 0;
    acceleration = false;
}

void Spaceship::tick()
{
    //rotate & check action========================================
    GLfloat rng = AngleRange(needAngle, dirAngle);
    if (fabs(rng) >= SPACESHIP_ROT_SPEED)
    {
        GLfloat dir = rng > 0 ? 1 : -1;
        setDirectionAngle(dirAngle + dir*SPACESHIP_ROT_SPEED);
    }
    else
    {
        setDirectionAngle(needAngle);
        applyAction();
    }

    //move control===============================================
    if (moveTimer.active())
    {
        if (moveTimer.get() > moveTimer.maxtag)
        {
           moveTimer.stop();
           acceleration = false;
        }
    }

    //acceleration===============================================
    if (acceleration)
    {
        PointF newSpeed = speed + PointF(SPACESHIP_ACCELEARTION * cos(dirAngle),
                                         SPACESHIP_ACCELEARTION * sin(dirAngle));
        if (newSpeed.abs() < SPACESHIP_MAX_SPEED)
        {
            speed = newSpeed;
        }
    }

    //figure behaviour===============================================
    Figure::tick();
}

//Called if target direction angle is reached
void Spaceship::applyAction()
{
    if (action == 's') //shot
    {
        shot();
    }
    else if (action == 'a') //accelerate
    {
        acceleration = true;
    }
    else if (action == 'm') //move
    {
        moveTimer.start((int)(actionParam * MOVE_DURATION_FACTOR));
        acceleration = true;
    }
    if (action == 'r') //rotate
    {
        //nothing
    }
    else if (action == 'f') //free action
    {
        moveTimer.stop();
        acceleration = false;
    }

    action = 0;
}

void Spaceship::setAction(char action, GLfloat angle, GLfloat param)
{
    if (!alive) return;

    this->action = action;
    this->actionParam = param;
    if (action != 'f') needAngle = angle;
}

void Spaceship::shot()
{
    if (!alive) return;

    PointF p = shotPoint();
    PointF sp( BULLET_SPEED * cos(dirAngle), BULLET_SPEED * sin(dirAngle));
    sp += speed;

    game->addActor(new Bullet(game, p, sp));
}

void Spaceship::die(PointF relativePoint)
{
    if (alive)
    {
        logi("Spaceship::die");

        crush(pos, ang(pos - relativePoint));

        game->addActor(new Explosion(game, pos));
        game->addActor(new Explosion(game, pos + PointF(SPACESHIP_SIZE/5, SPACESHIP_SIZE/5)));
        game->addActor(new Explosion(game, pos + PointF(-SPACESHIP_SIZE/5, -SPACESHIP_SIZE/5)));
        game->addActor(new Explosion(game, pos + PointF(SPACESHIP_SIZE/5, -SPACESHIP_SIZE/5)));
        game->addActor(new Explosion(game, pos + PointF(-SPACESHIP_SIZE/5, SPACESHIP_SIZE/5)));

        alive = false;
    }
}

void Spaceship::setDirectionAngle(GLfloat angle)
{
    if (dirAngle != angle)
    {
        while (angle > M_PI*2) angle -= M_PI*2;
        while (angle < -M_PI*2) angle += M_PI*2;

        dirAngle = angle;
        updateSelfVertices();
    }
}

void Spaceship::updateSelfVertices()
{
    Points points;
    GLfloat a;
    GLfloat r = SPACESHIP_SIZE / 2;

    a = dirAngle          ; points.push_back(PointF( r * cos(a), r * sin(a)) * 1.0f);
    a = dirAngle + dr(135); points.push_back(PointF( r * cos(a), r * sin(a)) * 0.7f);
    a = dirAngle + dr(180); points.push_back(PointF( r * cos(a), r * sin(a)) * 0.15f);
    a = dirAngle + dr(225); points.push_back(PointF( r * cos(a), r * sin(a)) * 0.7f);
    setSelfVertices(points);
}

void Spaceship::renderPos(IGraphicEngine *g, PointF localPos)
{
    //self==============================================
    Points triangles = toTriangles(localPos);
    Points verts = sceneVertices(localPos);

    g->fillTriangles(triangles, SPACESHIP_COLOR * 0.7);
    g->drawPolygon(verts, SPACESHIP_COLOR);

    //traction==============================================
    if (acceleration)
    {
        Points tractionTriangle;
        GLfloat a;
        GLfloat r = SPACESHIP_SIZE / 2;
        a = dirAngle + dr(180); tractionTriangle.push_back(localPos + PointF( r * cos(a), r * sin(a)) * 1.0f);
        a = dirAngle + dr(135); tractionTriangle.push_back(localPos + PointF( r * cos(a), r * sin(a)) * 0.2f);
        a = dirAngle + dr(225); tractionTriangle.push_back(localPos + PointF( r * cos(a), r * sin(a)) * 0.2f);

        g->fillTriangles(tractionTriangle, TRUNCATE_COLOR);
    }
}

void Spaceship::onCrush()
{
    game->defeat();
}
