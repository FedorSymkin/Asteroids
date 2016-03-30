#ifndef GEOMERTY_H_
#define GEOMERTY_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <android/log.h>

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "config.h"
#include <vector>

struct PointF
{
    GLfloat x;
    GLfloat y;

    PointF() : x(0), y(0) {}
    PointF(GLfloat vx, GLfloat vy) : x(vx), y(vy) {}

    PointF operator +(PointF other) {return PointF(x + other.x, y + other.y);}
    PointF operator -(PointF other) {return PointF(x - other.x, y - other.y);}
    PointF operator /(GLfloat v)    {return PointF(x / v, y / v);}
    PointF operator *(GLfloat v)    {return PointF(x*v, y*v);}

    void operator+=(PointF other)   {x += other.x; y += other.y;}
    void operator-=(PointF other)   {x -= other.x; y -= other.y;}
    void operator/=(PointF other)   {x /= other.x; y /= other.y;}
    void operator*=(PointF other)   {x *= other.x; y *= other.y;}
    void operator/=(GLfloat v)      {x /= v; y /= v;}
    void operator*=(GLfloat v)      {x *= v; y *= v;}

    bool operator ==(PointF other)  {return (this->x == other.x) && (this->y == other.y);}

    GLfloat abs()                   {return sqrt(x*x + y*y);}
    bool isAlmostZero()             {return ((fabs(x) < PRECISION) && (fabs(y) < PRECISION));}
};
typedef std::vector<PointF> Points;

GLfloat RangeF(PointF p1, PointF p2);
GLfloat ang(PointF p); //angle of vector (p.x, p.y)
GLfloat rtoline(PointF p1, PointF p2, PointF p); //range between line (p1,p2) and point p

void    calcMassCollision(GLfloat angle, PointF speed1, PointF speed2,
                            GLfloat mass1, GLfloat mass2,
                            PointF &newSpeed1, PointF &newSpeed2);

PointF  rotate_point(PointF p, GLfloat angle);
bool    pointInPolygon(Points &polygon, PointF point);
PointF  NearestPoint(Points &vertices, PointF point);
bool    intersectSegments(PointF P11, PointF P12, PointF P21, PointF P22, PointF *intersection = 0);
PointF  segmentShare(PointF p1, PointF p2, GLfloat share);
bool    LineIntersectPolygon(Points &polygon, PointF base, GLfloat lineAngle, unsigned int needCount, Points &res);
GLfloat AngleRange(GLfloat absAngle1, GLfloat absAngle2);
Points  makeSplinterPoints(Points &parentPoints, PointF pos, PointF orderbase, Points fractureLine, GLfloat &maxy);
#define middle(p1, p2) segmentShare(p1, p2, 0.5f)
#define dr(x) ((GLfloat)x * (GLfloat)M_PI / (GLfloat)180)



#endif
