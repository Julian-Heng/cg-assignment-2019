#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

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
} Camera;


Camera* makeCamera(void);

void getCameraViewMatrix(Camera*, mat4);

void updateCameraVectors(Camera*);
void doCameraForwardMovement(Camera*, float);
void doCameraLeftMovement(Camera*, float);
void doCameraBackwardMovement(Camera*, float);
void doCameraRightMovement(Camera*, float);
void doCameraMouseMovement(Camera*, double, double, bool);
void doCameraMouseScroll(Camera*, float);

#endif
