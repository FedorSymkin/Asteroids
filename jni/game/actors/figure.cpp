#include "game.h"
#include "actor.h"
#include "figure.h"


Figure::Figure(GameEngine *gameEngine, Points selfVert) :
    Actor::Actor(gameEngine),
    immediatelyKill(false)
{
	logi("Figure()");
    type |= TYPE_FIGURE;
    setSelfVertices(selfVert);
}

void Figure::setSelfVertices(Points &points)
{
    selfVertices = points;
    size = 0;
    for (unsigned int i = 0; i < selfVertices.size(); i++)
    {
        if (selfVertices[i].abs() > size) size = selfVertices[i].abs();
    }
}


/*
Здесь обрабатываются столкновения многоугольников.
Используется принцип соударения шаров. При этом в качестве координат
шаров принимаются не центры многоугольников, а непосредственно точки, которыми
многоугольники столкнулись
*/
void Figure::tick()
{
    for (unsigned int i = 0; i < game->actors.size(); i++)
    {
        if (game->actors[i]->getType() & TYPE_FIGURE )
        {
            Figure* other = (Figure*)game->actors[i];
            if (other != this)
            {
                this->saveParameters(); other->saveParameters();

                this->step(); other->step();
                if (collision(other))
                {
                    if (this->getType() & TYPE_SPACESHIP) ((Spaceship*)this)->die(other->pos);
                    else if (other->getType() & TYPE_SPACESHIP) ((Spaceship*)other)->die(this->pos);
                    else //Столкнулись 2 астероида
                    {
                        /*
                         *Приблизительно в 3% случаев коллизию не удаётся
                         *обработать так, чтобы избежать пересечения многоугольников.
                         *В этих случаях угол между точками взаимодействия корретируется
                         *на минимально возможную величину
                        */
                        GLfloat corrAngle = 0;
                        while (!updateSpeed(other, corrAngle))
                        {
                            if (corrAngle <= 0) corrAngle = -corrAngle + dr(1);
                            else corrAngle = -corrAngle;

                            if (fabs(corrAngle) > dr(180))
                            {
                                loge("actors collision: couldn't calc correct speed");
                                break;
                            }
                        }
                    }
                }

                this->restorePos(); other->restorePos();
            }
        }
    }
    step();
}


/*
 *Изменяет скорости многоугольников при заданном угле между "шарами"
 *и проверяет, позволило ли это разрешить коллизию
*/
bool Figure::updateSpeed(Figure* other, GLfloat corrAngle)
{
    this->restoreParameters(); other->restoreParameters();
    this->step(); other->step();

    if (!collisionProcessing(other, corrAngle))
    {
        logw("actors collision: couldn't get point in area. Speeds are not changed");
        return true;
    }

    this->restep(); other->restep();
    return !this->collision(other);
}

/*
 *Изменияет скорости многоугольков учитывая то, что шаг движения может
 *быть слишком большим. Если уменьшение шага до предельно низкого значения
 *не помогает - вернёт false
*/
bool Figure::collisionProcessing(Figure *other, GLfloat corrAngle)
{
    GLfloat sf = 1;
    GLfloat sfStep = 0.5;

    while (true)
    {
        bool hit =      this->doHit(other, corrAngle);
        if (!hit) hit = other->doHit(this, corrAngle);

        /*Если нет удара многоугольников - слишком большой шаг движения.
        Попытка уменьшить шаг и попробовать ещё раз*/
        if (!hit)
        {
            sf -= sfStep;
            if (sf <= PRECISION)
            {
                sfStep /= 2;
                if (sfStep < 0.0625) return false;
                sf = 1 - sfStep;
            }

            this->restep(sf);
            other->restep(sf);
            continue;
        }

        break;
    }

    return true;
}

/*
 *Изменияет скорости многоугольков:
 *Рассчитывает точки взаимодействия на двух многоугольниках и
 *применяет соударение шаров используя эти точки
 *Вернёт false если не удалось найти точки взаимодействи
 *
 *hitter - многоугольник, вершина которого попала внутрь многоугольника this
*/
bool Figure::doHit(Figure *hitter, GLfloat correction)
{
    Points SelfSceneVert = this->sceneVertices(hitter->pos);
    Points HitterSceneVert = hitter->sceneVertices(hitter->pos);

    for (unsigned int i = 0; i < hitter->verticesCount(); i++)
    {
        PointF &p = HitterSceneVert[i];

        if (pointInPolygon(SelfSceneVert, p))
        {
            PointF selfActionPoint = NearestPoint(SelfSceneVert, p);
            PointF hitterActionPoint = p;

            PointF selfNewSpeed, hitterNewSpeed;
            calcMassCollision(ang(selfActionPoint - hitterActionPoint) + correction,
                         this->speed, hitter->speed,
                         this->mass(), hitter->mass(),
                         selfNewSpeed, hitterNewSpeed);

            this->speed = selfNewSpeed;
            hitter->speed = hitterNewSpeed;

            return true;
        }
    }

    return false;
}

//Находится ли this в состоянии коллизии с любой другой Figure
//(включая те, которые войдут в игру при следующем tick)
bool Figure::collision()
{
    for (unsigned int i = 0; i < game->actors.size(); i++)
    {
        if (game->actors[i]->getType() & TYPE_FIGURE)
            if (game->actors[i] != this)
                if (collision((Figure*)game->actors[i])) return true;
    }

    for (unsigned int i = 0; i < game->actorsToAdd.size(); i++)
    {
        if (game->actorsToAdd[i]->getType() & TYPE_FIGURE)
            if (game->actorsToAdd[i] != this)
                if (collision((Figure*)game->actorsToAdd[i])) return true;
    }

    return false;
}


//Коллизией считается пересечение любых отрезков многоугольникв
bool Figure::collision(Figure* other)
{
    PointF op = other->scenePos(this->pos);
    GLfloat rng = RangeF(pos, op);
    if (rng > size + other->size) return false;
    if (this->willRemoved() || other->willRemoved()) return false;

    Points SelfSceneVert = this->sceneVertices(other->pos);
    Points OtherSceneVert = other->sceneVertices(other->pos);

    for (unsigned int i1 = 0, j1 = SelfSceneVert.size() - 1; i1 < SelfSceneVert.size(); j1 = i1++)
    {
        for (unsigned int i2 = 0, j2 = OtherSceneVert.size() - 1; i2 < OtherSceneVert.size(); j2 = i2++)
        {
            if (intersectSegments(SelfSceneVert[i1], SelfSceneVert[j1],
                                  OtherSceneVert[i2], OtherSceneVert[j2])) return true;
        }
    }

    return false;
}


void Figure::renderPos(IGraphicEngine *g, PointF localPos)
{
    ColorF color = immediatelyKill ? FIGURE_LITTLE_COLOR : FIGURE_BIG_COLOR;
    Points triangles = this->toTriangles(localPos);
    Points verts = this->sceneVertices(localPos);

    g->fillTriangles(triangles, color * 0.7);
    g->drawPolygon(verts, color);
}

void Figure::debugRender(IGraphicEngine *g)
{
    g->drawPoint(pos, ColorF(1,1,1));
}



PointF Figure::sceneVertex(PointF relativePoint, int i)
{
    return pos + getCyclicOffset(relativePoint) + selfVertices[i];
}

Points Figure::sceneVertices(PointF relativePoint)
{
    PointF offset = getCyclicOffset(relativePoint);

    Points points;
    for (unsigned int i = 0; i < verticesCount(); i++)
    {
        points.push_back(pos + offset + selfVertices[i]);
    }

    return points;
}

GLfloat Figure::mass()
{
    return perimeter();
}

GLfloat Figure::perimeter()
{
    GLfloat sum = 0;
    for (unsigned int i = 0, j = verticesCount() - 1; i < verticesCount(); j = i++)
    {
        PointF p1 = selfVertices[i];
        PointF p2 = selfVertices[j];

        sum += RangeF(p1,p2);
    }

    return sum;
}

/*
 *Разрушение фигуры на обломки производится по линии, заданной точкой
 *base и углом lineAngle. В некоторых случаях угол lineAngle не подходит для
 *корректного разрушения. В таких случаях он корректируется на минимально возможную
 *величину.
*/
void Figure::crush(PointF base, GLfloat lineAngle)
{
    if (willRemoved_) return;
    willRemoved_ = true;

    if (!immediatelyKill)
    {
        Points sceneVert = sceneVertices(base);
        GLfloat correction = 0;

        bool first = true;
        while (true)
        {
            if (!first)
            {
                if (correction <= 0) correction = -correction + dr(1);
                else correction = -correction;

                if (fabs(correction) > dr(180))
                {
                    logw("crush: couldn't calc");
                    break;
                }
            } else first = false;
            GLfloat angle = lineAngle + correction;


            /*Получить линию разлома, чтобы она пересекала
            строго 2 точки многоугольника*/
            Points fractureLine;
            if (!LineIntersectPolygon(sceneVert, base, angle, 2, fractureLine))
            {
                continue;
            }


            //Получить позиции новых осколков так, чтобы они находились
            //внутри новых многоугольников
            PointF center = middle(fractureLine[0], fractureLine[1]);
            Points perp;
            if (!LineIntersectPolygon(sceneVert, center, angle + dr(90), 2, perp))
            {
                continue;
            }
            PointF pos1 = middle(center, perp[0]);
            PointF pos2 = middle(center, perp[1]);

            /*Особенность: для того, чтобы не откалывались очень маленькие куски,
            соотношение размеров осколков не должно превышать определенную
            величину*/
            GLfloat yratio1;
            Points points1 = makeSplinterPoints(sceneVert, pos1, pos1, fractureLine, yratio1);
            GLfloat yratio2;
            Points points2 = makeSplinterPoints(sceneVert, pos2, pos2, fractureLine, yratio2);
            if ((yratio1/yratio2 > 5) || (yratio2/yratio1 > 5))
            {
                continue;
            }


            //Создание осколков с проверкой на то, чтобы они при создании
            //не пересекались с другими фигурами.
            //К новым осколкам добавляется небольшая скорость, направленная так,
            //чтобы они разлетались друг от друга.
            GLfloat SplinterSpeed = frand(SPLINTER_MAX_SPEED/2, SPLINTER_MAX_SPEED);

            Figure *f1 = new Figure(game, points1);
            f1->pos = pos1;
            PointF addspeed1 = (pos1 - center) / RangeF(pos1, center);
            addspeed1 *= SplinterSpeed;
            f1->speed = speed + addspeed1;
            f1->immediatelyKill = true;
            if (f1->collision())
            {
                delete f1;
                continue;
            }

            Figure *f2 = new Figure(game, points2);
            f2->pos = pos2;
            PointF addspeed2 = (pos2 - center) / RangeF(pos2, center);
            addspeed2 *= SplinterSpeed;
            f2->speed = speed + addspeed2;
            f2->immediatelyKill = true;
            if (f2->collision())
            {
                delete f1;
                delete f2;
                continue;
            }

            game->addActor(f1);
            game->addActor(f2);
            break;
        }
    }

    onCrush();
}


/*
 *
 *Все фигуры в игре соответствуют условиям:
 *1. Позиция многоугольника находится внутри него
 *2. Вершины перечислены в порядке увеличения или уменьшения угла
 *по отношению к позиции
 *
 *Поэтому для преобразования многоугольника в треугольники вместо алгоритма
 *триангуляции используется простой обход вершин
*/
Points Figure::toTriangles(PointF relativePoint)
{
    Points res;
    PointF actualPos = scenePos(relativePoint);

    for (unsigned int i = 0, j = selfVertices.size() - 1; i < selfVertices.size(); j = i++)
    {
        res.push_back(actualPos);
        res.push_back(actualPos + selfVertices[i]);
        res.push_back(actualPos + selfVertices[j]);
    }

    return res;
}


bool Figure::properVertices()
{
    return pointInPolygon(this->selfVertices, PointF(0,0));
}



