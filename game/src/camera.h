#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

#define ERR_CAMERA_MALLOC "Error: unable to allocate memory for camera\n"

#define JUMP_DURATION 0.75f
#define JUMP_HEIGHT 1.0f

#define JUMP_FORMULA(time) \
    ((JUMP_HEIGHT / (-(((JUMP_DURATION / 2.0f) - JUMP_DURATION) * \
    (JUMP_DURATION / 2.0f)))) * \
    -(((time) - JUMP_DURATION) * (time)))


typedef struct Camera
{
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float speed;
    float mouseSensitivity;
    float zoom;

    bool jumping;

    void (*getViewMatrix)(struct Camera*, mat4);

    void (*moveForward)(struct Camera*, float);
    void (*moveLeft)(struct Camera*, float);
    void (*moveBackward)(struct Camera*, float);
    void (*moveRight)(struct Camera*, float);
    void (*moveMouse)(struct Camera*, double, double, bool);
    void (*scrollMouse)(struct Camera*, float);

    void (*resetPosition)(struct Camera*);
    void (*resetFront)(struct Camera*);

    void (*setPosition)(struct Camera*, vec3);
    void (*setFront)(struct Camera*, vec3);
    void (*setJumping)(struct Camera*, bool);

    void (*poll)(struct Camera*);
    void (*jump)(struct Camera*);
} Camera;


Camera* newCamera(void);

#endif
