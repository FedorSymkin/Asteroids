#include "geometry.h"
#include "math.h"
#include "utils.h"


GLfloat RangeF(PointF p1, PointF p2)
{
    GLfloat dx = p2.x - p1.x;
    GLfloat dy = p2.y - p1.y;

    return sqrt(dx*dx + dy*dy);
}

//angle of vector (p.x, p.y)
GLfloat ang(PointF p)
{
    if (p.isAlmostZero()) return 0;

    GLfloat d = p.abs();

    GLfloat vsin = p.y / d;
    GLfloat vcos = p.x / d;

    GLfloat res = acos(vcos);
    if (vsin < 0) res = 2*M_PI - res;

    return res;
}

//range between line (p1,p2) and point p
GLfloat rtoline(PointF p1, PointF p2, PointF p)
{
    GLfloat y0 = p1.y;
    GLfloat y1 = p2.y;
    GLfloat y = p.y;

    GLfloat x0 = p1.x;
    GLfloat x1 = p2.x;
    GLfloat x = p.x;

    GLfloat c = ((y0 - y1)*x + (x1 - x0)*y + (x0*y1 - x1*y0));
    GLfloat z =  sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    return fabs(c/z);
}


void calcMassCollision(GLfloat angle, PointF speed1, PointF speed2,
                  GLfloat mass1, GLfloat mass2,
                  PointF &newSpeed1, PointF &newSpeed2)
{
    PointF rV1 = rotate_point(speed1, -angle);
    PointF rV2 = rotate_point(speed2, -angle);

    GLfloat rUx1 = ((mass1 - mass2)*rV1.x + 2*mass2*rV2.x)/(mass1 + mass2);
    GLfloat rUx2 = ((mass2 - mass1)*rV2.x + 2*mass1*rV1.x)/(mass1 + mass2);

    PointF rU1(rUx1, rV1.y);
    PointF rU2(rUx2, rV2.y);

    newSpeed1 = rotate_point(rU1, angle);
    newSpeed2 = rotate_point(rU2, angle);
}

PointF rotate_point(PointF p, GLfloat angle)
{
    if (p.isAlmostZero()) return p;

    GLfloat d = p.abs();
    GLfloat a = ang(p) + angle;
    return PointF( d * cos(a), d * sin(a) );
}


bool pointInPolygon(Points &polygon, PointF point)
{
    int ints = 0;

    for (unsigned int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        PointF p1c = polygon[i] - point;
        PointF p2c = polygon[j] - point;

        GLfloat maxy = fabs(p1c.y / (p2c.y - p1c.y));
        GLfloat xz = p1c.x + (p2c.x - p1c.x)*maxy;
        if ((xz >= 0) && (p1c.y * p2c.y <= 0))
        {
            ints++;
        }
    }

    return ((ints % 2) == 1);
}


PointF NearestPoint(Points &vertices, PointF point)
{
    PointF rp1;
    PointF rp2;

    GLfloat minRange = 0;
    for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
    {
        PointF p1 = vertices[i];
        PointF p2 = vertices[j];
        GLfloat r = rtoline(p1, p2, point);

        if ((i == 0) || (r < minRange))
        {
            rp1 = p1;
            rp2 = p2;
            minRange = r;
        }
    }

    GLfloat alfa = RangeF(point, rp1);
    GLfloat beta = RangeF(point, rp2);
    GLfloat ra = RangeF(rp1, rp2);
    GLfloat be = (ra*ra - alfa*alfa + beta*beta) / (2*ra);
    GLfloat aa = ra - be;
    GLfloat xx = fabs(aa/ra);

    return segmentShare(rp1, rp2, xx);
}

bool intersectSegments(PointF P11, PointF P12, PointF P21, PointF P22, PointF *intersection)
{
    GLfloat a = ang(P12 - P11);

    PointF P12c = rotate_point(P12 - P11, -a);
    PointF P21c = rotate_point(P21 - P11, -a);
    PointF P22c = rotate_point(P22 - P11, -a);

    GLfloat maxy = fabs(P21c.y / (P22c.y - P21c.y));
    GLfloat xz = P21c.x + (P22c.x - P21c.x)*maxy;

    if (intersection)
        *intersection = rotate_point(PointF(xz, 0), a) + P11;

    return ((xz >= 0) && (xz <= P12c.x) && (P21c.y * P22c.y <= 0));
}

PointF segmentShare(PointF p1, PointF p2, GLfloat share)
{
    return PointF(p1.x + (p2.x - p1.x)*share, p1.y + (p2.y - p1.y)*share);
}

bool LineIntersectPolygon(Points &polygon, PointF base, GLfloat lineAngle, unsigned int needCount, Points &res)
{
    GLfloat angle = lineAngle;
    for (unsigned int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        PointF p1 = rotate_point(polygon[i] - base, -angle);
        PointF p2 = rotate_point(polygon[j] - base, -angle);

        if (p1.y * p2.y <= 0)
        {
            GLfloat ys = fabs(p1.y / (p2.y - p1.y));
            GLfloat x = p1.x + (p2.x - p1.x)*ys;
            PointF p = rotate_point(PointF(x,0), angle) + base;
            res.push_back(p);
        }
    }

    return (res.size() == needCount) || (needCount == 0);
}

GLfloat AngleRange(GLfloat absAngle1, GLfloat absAngle2)
{
    GLfloat max = (absAngle1 >= absAngle2) ? absAngle1 : absAngle2;
    GLfloat min = (absAngle1 >= absAngle2) ? absAngle2 : absAngle1;
    GLfloat sign = (absAngle1 >= absAngle2) ? 1 : -1;
    GLfloat r = max - min;

    if (r < M_PI) return sign*r;
    else return -sign*(M_PI*2 - r);
}


struct PointFAngle
{
    PointF point;
    GLfloat angle;
};

Points makeSplinterPoints(Points &parentPoints, PointF pos, PointF orderbase, Points fractureLine, GLfloat &maxy)
{
    Points res;
    PointFAngle p;
    GLfloat fractureAngle = ang(fractureLine[1] - fractureLine[0]);

    //make the splinter points including fractureLine points
    vector<PointFAngle> points;
    PointF postr = rotate_point( pos - fractureLine[0], -fractureAngle);
    maxy = 0;
    for (unsigned int i = 0; i < parentPoints.size(); i++)
    {
        PointF Ptr = rotate_point( parentPoints[i] - fractureLine[0], -fractureAngle);
        if (((Ptr.y >= 0) && (postr.y >= 0)) || ((Ptr.y <= 0) && (postr.y < 0)))
        {
            if (maxy < fabs(Ptr.y)) maxy = fabs(Ptr.y);

            p.point = parentPoints[i];
            p.angle = ang(parentPoints[i] - orderbase);
            points.push_back(p);
        }
    }
    p.point = fractureLine[0];
    p.angle = ang(fractureLine[0] - orderbase);
    points.push_back(p);

    p.point = fractureLine[1];
    p.angle = ang(fractureLine[1] - orderbase);
    points.push_back(p);


    //sort points by angle and make result
    for (unsigned int i = 0; i < points.size(); i++)
    {
        int min = 0;
        for (unsigned int j = i; j < points.size(); j++)
        {
            if ((j == i) || (points[j].angle < points[min].angle)) min = j;
        }

        PointFAngle p = points[min];
        points[min] = points[i];
        points[i] = p;
        res.push_back((p.point - pos)*0.8);
    }

    return res;
}

