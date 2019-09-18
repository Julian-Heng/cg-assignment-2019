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

static void setPosition(Camera*, vec3);
static void setFront(Camera*, vec3);
static void setJumping(Camera*, bool);

static void resetPosition(Camera*);
static void resetFront(Camera*);

static void poll(Camera*);
static void jump(Camera*);

static void updateCameraVectors(Camera*);


Camera* newCamera()
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

    cam->setPosition = setPosition;
    cam->setFront = setFront;
    cam->setJumping = setJumping;

    cam->resetPosition = resetPosition;
    cam->resetFront = resetFront;

    cam->poll = poll;
    cam->jump = jump;
}


static void getViewMatrix(Camera* this, mat4 result)
{
    vec3 temp;
    glm_vec3_add(this->position, this->front, temp);
    glm_lookat(this->position, temp, this->up, result);
}


static void moveForward(Camera* this, float timeDelta)
{
    vec3 temp;
    glm_vec3_copy((vec3){this->front[0], 0.0f, this->front[2]}, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, this->speed * timeDelta, temp);
    glm_vec3_add(temp, this->position, this->position);
}


static void moveLeft(Camera* this, float timeDelta)
{
    vec3 temp;
    glm_vec3_scale(this->right, this->speed * timeDelta, temp);
    glm_vec3_sub(this->position, temp, this->position);
}


static void moveBackward(Camera* this, float timeDelta)
{
    vec3 temp;
    glm_vec3_copy((vec3){this->front[0], 0.0f, this->front[2]}, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, this->speed * timeDelta, temp);
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


static void setPosition(Camera* this, vec3 newPos)
{
    glm_vec3_copy(newPos, this->position);
}


static void setFront(Camera* this, vec3 newPos)
{
    glm_vec3_copy(newPos, this->front);
}


static void setJumping(Camera* this, bool value)
{
    this->jumping = value;
}


static void resetPosition(Camera* this)
{
    this->setPosition(this, (vec3){0.0f, 0.0f, 3.0f});
}


static void resetFront(Camera* this)
{
    this->setFront(this, (vec3){0.0f, 0.0f, 3.0f});
    this->yaw = -90.0f;
    this->pitch = 0.0f;
    updateCameraVectors(this);
}


static void poll(Camera* this)
{
    this->jump(this);
}


static void jump(Camera* this)
{
    static bool start = true;
    static float startTime = 0.0f;
    static float initialPosition = 0.0f;

    float deltaTime;

    if (! this->jumping)
    {
        return;
    }

    if (start)
    {
        start = false;
        startTime = glfwGetTime();
        initialPosition = this->position[1];
        return;
    }

    deltaTime = glfwGetTime() - startTime;
    this->position[1] = initialPosition + JUMP_FORMULA(deltaTime);

    if (this->position[1] - initialPosition < 0.0f)
    {
        start = true;
        startTime = 0.0f;
        initialPosition = 0.0f;
        this->position[1] = initialPosition;
        this->setJumping(this, false);
    }
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
