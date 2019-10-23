#include <stdio.h>
#include <stdarg.h>

#include "game.h"
#include "camera.h"

#include "log.h"

static void _logInfo(FILE*, int*, char*, ...);

void logInfo(FILE* f, Backend* engine)
{
    static unsigned long long frameCount = 0;
    static unsigned int frameDelta = 0;
    static float fpsLastTime = 0.0f;
    static unsigned int cacheFrameDelta;
    static float cacheFrameLatency;
    static bool first = true;
    static int rows = 0;

    Camera* cam;

    if (! engine)
    {
        return;
    }

    cam = engine->cam;
    frameDelta++;
    frameCount++;

    if ((glfwGetTime() - fpsLastTime) >= 1.0)
    {
        cacheFrameDelta = frameDelta;
        cacheFrameLatency = 1000.0 / (double)(cacheFrameDelta);

        frameDelta = 0;
        fpsLastTime += 1.0f;
    }

    if (! first)
    {
        fprintf(f, "\e[%dA", rows);
        rows = 0;
    }

    _logInfo(f, &rows, LOG_CLEAR LOG_PROJECTION_TYPE "\n",
        engine->options[GAME_USE_PERSPECTIVE] ? "Perspective" : "Orthographic");
    _logInfo(f, &rows, LOG_CLEAR LOG_PLAYER_STATE "\n", engine->options[GAME_PLAYER_DIE] ? "Dead (Press R to restart)" : "Alive");
    _logInfo(f, &rows, LOG_CLEAR LOG_GAME_STATE "\n", engine->options[GAME_WIN] ? "Win (Press R to restart)" : "Active");
    _logInfo(f, &rows, LOG_CLEAR LOG_RESOLUTION "\n", engine->width,
                                                      engine->height);
    _logInfo(f, &rows, LOG_CLEAR LOG_LIGHT_LEVEL "\n", engine->lightLevel);
    _logInfo(f, &rows, LOG_CLEAR LOG_FRAME_COUNT "\n", frameCount);
    _logInfo(f, &rows, LOG_CLEAR LOG_FPS "\n", cacheFrameDelta);
    _logInfo(f, &rows, LOG_CLEAR LOG_FRAME_LATENCY "\n", cacheFrameLatency);
    _logInfo(f, &rows, LOG_CLEAR LOG_CAM_LOCATION "\n", cam->position[0],
                                                        cam->position[1],
                                                        cam->position[2]);
    _logInfo(f, &rows, LOG_CLEAR LOG_CAM_FRONT "\n", cam->front[0],
                                                     cam->front[1],
                                                     cam->front[2]);
    _logInfo(f, &rows, LOG_CLEAR LOG_CAM_YAW "\n", cam->yaw);
    _logInfo(f, &rows, LOG_CLEAR LOG_CAM_PITCH "\n", cam->pitch);

    first = false;
}


static void _logInfo(FILE* f, int* count, char* fmt, ...)
{
    va_list(args);
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    (*count)++;
}
