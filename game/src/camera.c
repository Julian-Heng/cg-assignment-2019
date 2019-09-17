#include <cglm/mat4.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>

#include <math.h>
#include <stdio.h>
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
        fprintf(stderr, ERR_CAMERA_MALLOC);
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


static void getViewMatrix(Camera* this, mat4 result)
{
    vec3 temp;
    glm_vec3_add(this->position, this->front, temp);
    glm_lookat(this->position, temp, this->up, result);
}


static void moveForward(Camera* this, float timeDelta)
{
    glm_vec3_muladds(this->front, this->speed * timeDelta, this->position);
}


static void moveLeft(Camera* this, float timeDelta)
{
    float velocity = this->speed * timeDelta;
    vec3 temp;
    glm_vec3_scale(this->right, velocity, temp);
    glm_vec3_sub(this->position, temp, this->position);
}


static void moveBackward(Camera* this, float timeDelta)
{
    float velocity = this->speed * timeDelta;
    vec3 temp;
    glm_vec3_scale(this->front, velocity, temp);
    glm_vec3_sub(this->position, temp, this->position);
}


static void moveRight(Camera* this, float timeDelta)
{
    glm_vec3_muladds(this->right, this->speed * timeDelta, this->position);
}


static void moveMouse(Camera* this, double xoffset,
                           double yoffset, bool constraint)
{
    this->yaw += xoffset * this->mouseSensitivity;
    this->pitch += yoffset * this->mouseSensitivity;
    this->pitch = constraint && this->pitch > 89.0f ? 89.0f : this->pitch;
    this->pitch = constraint && this->pitch < -89.0f ? -89.0f : this->pitch;

    updateCameraVectors(this);
}


static void scrollMouse(Camera* this, float yoffset)
{
    this->zoom -= RANGE_INC(this->zoom, 1.0f, 45.0f) ? yoffset : 0;
    this->zoom = MAX(this->zoom, 1.0f);
    this->zoom = MIN(this->zoom, 45.0f);
}


static void updateCameraVectors(Camera* this)
{
    glm_vec3_normalize_to((vec3){
        cos(glm_rad(this->yaw)) * cos(glm_rad(this->pitch)),
        sin(glm_rad(this->pitch)),
        sin(glm_rad(this->yaw)) * cos(glm_rad(this->pitch))
    }, this->front);
    glm_vec3_crossn(this->front, this->worldUp, this->right);
    glm_vec3_crossn(this->right, this->front, this->up);
}
