#ifndef UTILS_H_
#define UTILS_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <android/log.h>

#include "stdio.h"
#include "stdlib.h"
#include "geometry.h"
#include "config.h"
#include <vector>
using namespace std;

GLfloat frand(GLfloat min, GLfloat max);
int     irand(int min, int max);
void    randomize(int r);
void    randomize();

struct ColorF
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;

    ColorF() : r(0), g(0), b(0), a(1.0f) {}
    ColorF(GLfloat vr, GLfloat vg, GLfloat vb) : r(vr), g(vg), b(vb), a(1.0f) {}
    ColorF(GLfloat vr, GLfloat vg, GLfloat vb, GLfloat va) : r(vr), g(vg), b(vb), a(va) {}

    ColorF operator* (GLfloat v) {return ColorF(r*v, g*v, b*v, a);}
};

class Timer
{
    private:
        static double getCurrentTime();
        double tm;
        bool activeFlag;

    public:
        Timer() : tm(getCurrentTime()), activeFlag(false) {}

        void    start(double m = 0){clear(); activeFlag = true; this->maxtag = m;}
        void    stop()             {activeFlag = false;}
        bool    active()           {return activeFlag;}

        GLfloat get()              {return activeFlag ? getCurrentTime() - tm : 0;}
        void    clear()            {tm = getCurrentTime();}
        double  maxtag;
};



#define  logi(...)  {__android_log_print(ANDROID_LOG_INFO, "asteriods", __VA_ARGS__);}
#define  logw(...)  {__android_log_print(ANDROID_LOG_WARN, "asteriods", __VA_ARGS__);}
#define  loge(...)  {__android_log_print(ANDROID_LOG_ERROR, "asteriods", __VA_ARGS__);}


#endif
