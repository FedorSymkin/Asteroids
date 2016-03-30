#ifndef CONFIG_H
#define CONFIG_H

//Figure parameters
#define FIGURE_MIN_SIZE             0.2500f
#define FIGURE_MAX_SIZE             0.6000f
#define MAX_INITIAL_FIGURE_SPEED    0.0100f
#define FIGURE_MIN_VERTICES_COUNT   3
#define FIGURE_MAX_VERTICES_COUNT   10


//Game parameters
#define INITIAL_LEVEL               0
#define MAX_LEVEL                   6
#define INITIAL_LIVES               10
#define SPACESHIP_SIZE              0.4500f
#define BULLET_SPEED                0.0750f
#define SPLINTER_MAX_SPEED          0.0035f
#define SPACESHIP_ACCELEARTION      0.0016f
#define SPACESHIP_ROT_SPEED         0.2000f
#define BULLET_MAX_MOVE             1.3000f
#define SPACESHIP_MAX_SPEED         0.0380f
#define PLAYER_SAFE_ZONE            0.5000f
#define STD_PAUSE                   3000
#define ACCELERATE_TOUT             250
#define MOVE_DURATION_FACTOR        400


//Render parameters
#define BULLET_RADIUS               0.0100f
#define EXPLOSION_SPEED             0.0150f
#define EXPLOSION_RADIUS            0.0800f
#define BACKGROUND_COLOR            ColorF(0.0f, 0.0f, 0.0f)
#define SPACESHIP_COLOR             ColorF(0.4f, 0.4f, 0.9f)
#define TRUNCATE_COLOR              ColorF(1.0f, 0.3f, 0.4f)
#define FIGURE_BIG_COLOR            ColorF(0.8f, 0.9f, 0.8f)
#define FIGURE_LITTLE_COLOR         ColorF(0.5f, 0.5f, 0.5f)
#define BULLET_COLOR                ColorF(1.0f, 1.0f, 1.0f)
#define EXPLOSION_COLOR             ColorF(1.0f, 0.0f, 0.0f)
#define LIVE_PICT_COLOR             ColorF(0.0f, 0.0f, 1.0f)
#define LIVE_PICT_SIZE              0.0500f

//System parameters
#define TICK_TIME                   25
#define PRECISION                   0.0000001f
//#define ASTERIODS_DEBUG



#endif // CONFIG_H
