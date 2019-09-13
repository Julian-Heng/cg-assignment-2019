#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

typedef enum Movement
{
    FORWARD = GLFW_KEY_W,
    LEFT = GLFW_KEY_A,
    BACKWARD = GLFW_KEY_S,
    RIGHT = GLFW_KEY_D
} Movement;


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

    void (*getViewMatrix)(struct Camera*, mat4);

    void (*moveForward)(struct Camera*, float);
    void (*moveLeft)(struct Camera*, float);
    void (*moveBackward)(struct Camera*, float);
    void (*moveRight)(struct Camera*, float);
    void (*moveMouse)(struct Camera*, double, double, bool);
    void (*scrollMouse)(struct Camera*, float);
} Camera;


Camera* makeCamera(void);

#endif
