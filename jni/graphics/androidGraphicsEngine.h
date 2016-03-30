#ifndef ANDROID_GRAPHICS_ENGINE_H_
#define ANDROID_GRAPHICS_ENGINE_H_

#include "utils.h"
#include "graphicsEngine.h"

#define VERTICES_BUFFER_COUNT 50
#define CIRCLE_VERTICES_COUNT 10

class AndroidGraphicEngine : public IGraphicEngine
{
public:
    void init(int width, int height);
    void deinit();

    virtual void clear(ColorF color);
    virtual void drawLine(PointF p1, PointF p2, ColorF color);
    virtual void drawPolyline(Points &vertices, ColorF color);
    virtual void drawPolygon(Points &vertices, ColorF color);
    virtual void drawPoint(PointF p, ColorF color, GLfloat size = 0.015);
    virtual void drawCircle(PointF center, GLfloat radius, ColorF color, bool filled = false);
    virtual void fillTriangles(Points &vertices, ColorF color);


    virtual ~AndroidGraphicEngine();
private:
    GLuint glprogram;

    GLfloat matrix[16];
    GLuint  uMartixLocation;
    GLuint  aPositionLocation;
    GLuint  aColorLocation;

    void makeMatrix(int screenWidth, int screenHeight);
    void drawPrimitive(Points &vertices, ColorF color, GLenum type, bool cycledVertices = false);
};

#endif
