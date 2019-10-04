#ifndef LOG_H
#define LOG_H

#include "game.h"

#define LOG_CLEAR           "\r\e[2K"
#define LOG_RESOLUTION      "Resolution          : %d x %d"
#define LOG_FRAME_COUNT     "Frame count         : %lld"
#define LOG_FPS             "Framerate           : %d fps"
#define LOG_FRAME_LATENCY   "Latency             : %f ms"
#define LOG_CAM_LOCATION    "Camera position     : (%f, %f, %f)"
#define LOG_CAM_FRONT       "Camera front vector : (%f, %f, %f)"

void logInfo(FILE*, Backend*);

#endif
