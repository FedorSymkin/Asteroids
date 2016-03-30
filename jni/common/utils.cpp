#include "utils.h"
#include <sys/time.h>
#include <time.h>
#include "math.h"

double Timer::getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec*1000 + (double)tv.tv_usec/1000;
}

GLfloat frand(GLfloat min, GLfloat max)
{
    return min + (max - min)*((GLfloat)rand() / (GLfloat)RAND_MAX);
}

int irand(int min, int max)
{
    return rand() % (max - min) + min;
}

void randomize(int r)
{
    srand(r);
}

void randomize()
{
    srand(time(NULL));
}
