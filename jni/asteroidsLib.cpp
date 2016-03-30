#include <jni.h>
#include "androidGraphicsEngine.h"

#include "game.h"
#include "utils.h"
#include "stdlib.h"
#include "stdio.h"

AndroidGraphicEngine *graphicEngine = 0;
GameEngine *gameEngine = 0;



extern "C"
{
	JNIEXPORT void JNICALL Java_com_asteroids_AsteroidsLib_init(JNIEnv* env, jclass obj, jint width, jint height)
	{
        logi("AsteroidsLib init width = %d height = %d", width, height);

		if (!graphicEngine)
		{
            graphicEngine = new AndroidGraphicEngine;
			graphicEngine->init(width, height);
		}

		if (!gameEngine)
		{
            gameEngine = new GameEngine(width, height);
            gameEngine->init();
		}
	}
	
	JNIEXPORT void JNICALL Java_com_asteroids_AsteroidsLib_tick(JNIEnv* env, jclass obj)
	{
		if (gameEngine && graphicEngine)
		{
            gameEngine->tick();
			gameEngine->render(graphicEngine);
		}
	}

    JNIEXPORT void JNICALL Java_com_asteroids_AsteroidsLib_touchevent(JNIEnv* env, jclass obj, jboolean down, jfloat x, jfloat y)
	{
		if (gameEngine)
		{
            gameEngine->touchEvent(PointF(x,y), down);
		}
	}

    JNIEXPORT void JNICALL Java_com_asteroids_AsteroidsLib_keyevent(JNIEnv* env, jclass obj, jboolean down, jint key)
    {
        if (gameEngine)
        {
            gameEngine->keyEvent(key, down);
        }
    }

	JNIEXPORT void JNICALL Java_com_asteroids_AsteroidsLib_deinit(JNIEnv* env, jclass obj)
	{
        logi("AsteroidsLib deinit");

		if (gameEngine)
		{
			gameEngine->deinit();
			delete gameEngine;
			gameEngine = 0;
		}

		if (graphicEngine)
		{
			graphicEngine->deinit();
			delete graphicEngine;
			graphicEngine = 0;
		}
    }

	JNIEXPORT jint JNICALL Java_com_asteroids_AsteroidsLib_avgticktime(JNIEnv* env, jclass obj)
	{
		if (gameEngine) return gameEngine->avgTickTime();
		else return 0;
	}

	JNIEXPORT jint JNICALL Java_com_asteroids_AsteroidsLib_maxticktime(JNIEnv* env, jclass obj)
	{
		if (gameEngine) return gameEngine->maxTickTime();
		else return 0;
	}
}
