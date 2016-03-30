#include "androidGraphicsEngine.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "utils.h"
#include <stdlib.h>


void checkGlError(const char* operation)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		loge("%s: opengl error (0x%X)\n", operation, error);
	}
}


bool checkGlShaderCompile(GLenum shaderType, GLuint shader)
{
	logi("checkGlShaderCompile...");

    GLint compiled = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    checkGlError("glGetShaderiv");

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        checkGlError("glGetShaderiv");

        if (infoLen)
        {
            char* buf = (char*) malloc(infoLen);
            if (buf)
            {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                checkGlError("glGetShaderInfoLog");

                loge("Could not compile shader %d:\n%s\n",shaderType, buf);
                free(buf);
            } else loge("malloc error");
        } else loge("no opengl error message");

        loge("checkGlShaderCompile error");
        return false;
    }

    logi("checkGlShaderCompile OK");
    return true;
}

bool checkGlProgramLink(GLuint program)
{
	logi("checkGlProgramLink...");

    GLint linkStatus = GL_FALSE;

    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    checkGlError("glGetProgramiv");

    if (linkStatus != GL_TRUE)
    {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        checkGlError("glGetProgramiv");

        if (bufLength)
        {
            char* buf = (char*) malloc(bufLength);
            if (buf)
            {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                loge("Could not link program:\n%s\n", buf);
                free(buf);
            } else loge("malloc error");
        } else loge("no opengl error message");

        logi("checkGlProgramLink error");
        return false;
    }

    logi("checkGlProgramLink OK");
    return true;
}

GLuint makeGLShader(GLenum shaderType, const char *source)
{
    logi("makeGLShader");

    GLuint shader = glCreateShader(shaderType);
    checkGlError("glCreateShader");

    if (shader)
    {
        glShaderSource(shader, 1, &source, NULL);
        checkGlError("glShaderSource");

        glCompileShader(shader);
        checkGlError("glCompileShader");

        if (checkGlShaderCompile(shaderType, shader))
        {
        	logi("makeGLShader OK");
            return shader;
        }
        else
        {
            glDeleteShader(shader);
            return 0;
        }
    } else return 0;
}

GLuint makeGLProgram(const char *vertexShaderSource, const char *fragmentShaderSource)
{
	logi("makeGLProgram");

    GLuint vertexShader = makeGLShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vertexShader) return 0;

    GLuint fragmentShader = makeGLShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!fragmentShader) return 0;

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");

        glAttachShader(program, fragmentShader);
        checkGlError("glAttachShader");

        glLinkProgram(program);
        checkGlError("glLinkProgram");

        if (checkGlProgramLink(program))
        {
        	logi("makeGLProgram OK");
            return program;
        }
        else
        {
            glDeleteProgram(program);
            return 0;
        }
    } else return 0;
}




GLuint makeSimpleGLProgram()
{
    logi("makeSimpleGLProgram");

    char vSource[] = "attribute vec4 aPosition;\n"
                     "attribute vec4 aColor;\n"
                     "varying vec4 vColor;\n"
                     "uniform mat4 uMatrix;\n"
                     "void main() {\n"
                     "vColor = aColor;\n"
                     "gl_Position = uMatrix * aPosition;\n"
                     "}";

    char fSource[] = "precision mediump float;\n"
                     "varying vec4 vColor;\n"
                     "void main() {\n"
                     "gl_FragColor = vColor;\n"
                     "}";

    logi("vSource = %s", vSource);
    logi("fSource = %s", fSource);

    return makeGLProgram(vSource, fSource);
}

void AndroidGraphicEngine::init(int width, int height)
{
	logi("AndroidGraphicEngine::init...");

    glprogram = makeSimpleGLProgram();
    if (!glprogram) loge("Error makeSimpleGLProgram");

    glUseProgram(glprogram);
        checkGlError("glUseProgram");

    uMartixLocation = glGetUniformLocation(glprogram, "uMatrix");
        checkGlError("glGetUniformLocation");
        logi("uMartixLocation = %d", uMartixLocation);

    aPositionLocation = glGetAttribLocation(glprogram, "aPosition");
        checkGlError("glGetAttribLocation");
        logi("aPositionLocation = %d", aPositionLocation);

    aColorLocation = glGetAttribLocation(glprogram, "aColor");
        checkGlError("glGetAttribLocation");
        logi("aColorLocation = %d", aColorLocation);

    makeMatrix(width, height);

    glUniformMatrix4fv(uMartixLocation, 1, GL_FALSE, matrix);
        checkGlError("glUniformMatrix4fv");

    glViewport(0, 0, width, height);
        checkGlError("glViewport");

    logi("AndroidGraphicEngine::init finished");
}

void AndroidGraphicEngine::makeMatrix(int screenWidth, int screenHeight)
{
    //identity matrix
    memset(matrix, 0, 16 * sizeof(GLfloat));
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;


    GLfloat aspectRatio = (GLfloat)screenHeight / (GLfloat)screenWidth;

    if (aspectRatio > 1)
    {
        matrix[5] /= aspectRatio; //correct y
    }
    else if (aspectRatio < 1)
    {
        matrix[0] /= aspectRatio; //correct x
    }

    logi("matrix = ");
    for (int i = 0; i < 4; i++)
        logi("%f \t %f \t %f \t %f", matrix[i*4], matrix[i*4+1], matrix[i*4+2], matrix[i*4+3]);
}


AndroidGraphicEngine::~AndroidGraphicEngine()
{
    deinit();
}


void AndroidGraphicEngine::deinit()
{
	logi("AndroidGraphicEngine::deinit...");

    if (glprogram)
    {
        glDeleteProgram(glprogram);
        glprogram = 0;
    }

    logi("AndroidGraphicEngine::deinit finished");
}

void AndroidGraphicEngine::drawPrimitive(Points &vertices, ColorF color, GLenum type, bool cycledVertices)
{
    int cnt = cycledVertices ? vertices.size() + 1 : vertices.size();
    if (cnt > VERTICES_BUFFER_COUNT)
    {
        loge("drawPrimitive: loo much vertices count");
        return;
    }

    GLfloat vVertices[VERTICES_BUFFER_COUNT*2];
    GLfloat vColor[VERTICES_BUFFER_COUNT*4];

    int i;
    for (i = 0; i < vertices.size(); i++)
    {
        vVertices[i*2] = vertices[i].x;
        vVertices[i*2 + 1] = vertices[i].y;

        vColor[i*4] = color.r;
        vColor[i*4 + 1] = color.g;
        vColor[i*4 + 2] = color.b;
        vColor[i*4 + 3] = color.a;
    }

    if (cycledVertices)
    {
        vVertices[i*2] = vertices[0].x;
        vVertices[i*2 + 1] = vertices[0].y;

        vColor[i*4] = color.r;
        vColor[i*4 + 1] = color.g;
        vColor[i*4 + 2] = color.b;
        vColor[i*4 + 3] = color.a;
    }

    glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, vVertices);
        checkGlError("glVertexAttribPointer");

    glVertexAttribPointer(aColorLocation, 4, GL_FLOAT, GL_FALSE, 0, vColor);
        checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(aPositionLocation);
        checkGlError("glEnableVertexAttribArray");
    glEnableVertexAttribArray(aColorLocation);
        checkGlError("glEnableVertexAttribArray");

    glDrawArrays(type, 0, cnt);
        checkGlError("glDrawArrays");

    glDisableVertexAttribArray(aPositionLocation);
        checkGlError("glDisableVertexAttribArray");
    glDisableVertexAttribArray(aColorLocation);
        checkGlError("glDisableVertexAttribArray");
}

void AndroidGraphicEngine::clear(ColorF color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    checkGlError("glClearColor");

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
}

//=============================================================================

void AndroidGraphicEngine::drawLine(PointF p1, PointF p2, ColorF color)
{
    Points points;
    points.push_back(p1);
    points.push_back(p2);

    drawPrimitive(points, color, GL_LINES);
}

void AndroidGraphicEngine::drawPolyline(Points &vertices, ColorF color)
{
    drawPrimitive(vertices, color, GL_LINE_STRIP);
}

void AndroidGraphicEngine::drawPolygon(Points &vertices, ColorF color)
{
    drawPrimitive(vertices, color, GL_LINE_STRIP, true);
}

void AndroidGraphicEngine::drawPoint(PointF p, ColorF color, GLfloat size)
{
    Points points;
    points.push_back(PointF(p.x-size,   p.y-size));
    points.push_back(PointF(p.x-size,   p.y+size));
    points.push_back(PointF(p.x+size,   p.y+size));
    points.push_back(PointF(p.x+size,   p.y-size));

    drawPolygon(points, color);
}

void AndroidGraphicEngine::drawCircle(PointF center, GLfloat radius, ColorF color, bool filled)
{
    GLfloat angleStep = M_PI*2/CIRCLE_VERTICES_COUNT;
    Points points;
    for (GLfloat a = 0; a < M_PI*2; a += angleStep)
    {
        points.push_back(center + PointF(radius * cos(a),  radius * sin(a)));
    }

    if (filled)
        drawPrimitive(points, color, GL_TRIANGLE_FAN);
    else
        drawPrimitive(points, color, GL_LINE_STRIP, true);
}

void AndroidGraphicEngine::fillTriangles(Points &vertices, ColorF color)
{
    drawPrimitive(vertices, color, GL_TRIANGLES);
}






