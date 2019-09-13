#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/cam.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "camera.h"


static void linkMethods(Camera*);

static void getViewMatrix(Camera*, mat4);
static void moveForward(Camera*, float);
static void moveLeft(Camera*, float);
static void moveBackward(Camera*, float);
static void moveRight(Camera*, float);
static void moveMouse(Camera*, double, double, bool);
static void scrollMouse(Camera*, float);

static void updateCameraVectors(Camera*);

Camera* makeCamera()
{
    Camera* cam;

    if (! (cam = (Camera*)malloc(sizeof(Camera))))
    {
        return NULL;
    }

    memset(cam, 0, sizeof(Camera));
    linkMethods(cam);

    glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, cam->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, cam->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cam->up);
    glm_vec3_zero(cam->right);
    glm_vec3_copy(cam->up, cam->worldUp);

    cam->yaw = -90.0f;
    cam->pitch = 0.0f;

    cam->speed = 3.0f;
    cam->mouseSensitivity = 0.05f;
    cam->zoom = 45.0f;

    updateCameraVectors(cam);

    return cam;
}


static void linkMethods(Camera* cam)
{
    cam->getViewMatrix = getViewMatrix;
    cam->moveForward = moveForward;
    cam->moveLeft = moveLeft;
    cam->moveBackward = moveBackward;
    cam->moveRight = moveRight;
    cam->moveMouse = moveMouse;
    cam->scrollMouse = scrollMouse;
}


static void getViewMatrix(Camera* cam, mat4 result)
{
    vec3 temp;
    glm_vec3_add(cam->position, cam->front, temp);
    glm_lookat(cam->position, temp, cam->up, result);
}


static void moveForward(Camera* cam, float timeDelta)
{
    glm_vec3_muladds(cam->front, cam->speed * timeDelta, cam->position);
}


static void moveLeft(Camera* cam, float timeDelta)
{
    float velocity = cam->speed * timeDelta;
    vec3 temp;
    glm_vec3_scale(cam->right, velocity, temp);
    glm_vec3_sub(cam->position, temp, cam->position);
}


static void moveBackward(Camera* cam, float timeDelta)
{
    float velocity = cam->speed * timeDelta;
    vec3 temp;
    glm_vec3_scale(cam->front, velocity, temp);
    glm_vec3_sub(cam->position, temp, cam->position);
}


static void moveRight(Camera* cam, float timeDelta)
{
    glm_vec3_muladds(cam->right, cam->speed * timeDelta, cam->position);
}


static void moveMouse(Camera* cam, double xoffset,
                           double yoffset, bool constraint)
{
    cam->yaw += xoffset * cam->mouseSensitivity;
    cam->pitch += yoffset * cam->mouseSensitivity;
    cam->pitch = constraint && cam->pitch > 89.0f ? 89.0f : cam->pitch;
    cam->pitch = constraint && cam->pitch < -89.0f ? -89.0f : cam->pitch;

    updateCameraVectors(cam);
}


static void scrollMouse(Camera* cam, float yoffset)
{
    cam->zoom -= RANGE_INC(cam->zoom, 1.0f, 45.0f) ? yoffset : 0;
    cam->zoom = MAX(cam->zoom, 1.0f);
    cam->zoom = MIN(cam->zoom, 45.0f);
}


static void updateCameraVectors(Camera* cam)
{
    glm_vec3_normalize_to((vec3){
        cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch)),
        sin(glm_rad(cam->pitch)),
        sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch))
    }, cam->front);
    glm_vec3_crossn(cam->front, cam->worldUp, cam->right);
    glm_vec3_crossn(cam->right, cam->front, cam->up);
}
