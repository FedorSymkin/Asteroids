#ifndef GRAPHICS_ENGINE_H_
#define GRAPHICS_ENGINE_H_

#include "utils.h"

class IGraphicEngine
{
public:
    virtual void init(int width, int height) = 0;
    virtual void deinit() = 0;

    virtual void clear(ColorF color) = 0;
    virtual void drawLine(PointF p1, PointF p2, ColorF color) = 0;
    virtual void drawPolyline(Points &vertices, ColorF color) = 0;
    virtual void drawPolygon(Points &vertices, ColorF color) = 0;
    virtual void drawPoint(PointF p, ColorF color, GLfloat size = 0.015) = 0;
    virtual void drawCircle(PointF center, GLfloat radius, ColorF color, bool filled = false) = 0;
    virtual void fillTriangles(Points &vertices, ColorF color) = 0;

    virtual ~IGraphicEngine() {}
};

#endif
