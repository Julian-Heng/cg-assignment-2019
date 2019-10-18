#include <cglm/mat4.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <cglm/affine.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "box.h"
#include "list.h"
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

static void attach(Camera*, Box*);
static void detach(Camera*);

static void setPosition(Camera*, vec3);
static void setFront(Camera*, vec3);
static void setJump(Camera*, bool);

static void recordInitialPosition(Camera*);
static void resetPosition(Camera*);
static void resetFront(Camera*);

static void poll(Camera*);
static void destroy(Camera*);

static void updateCameraVectors(Camera*);
static void jump(Camera*);

static float calcJump(float t);
static float _calcJump(float t);


Camera* newCamera(vec3 position)
{
    Camera* cam;

    if (! (cam = (Camera*)malloc(sizeof(Camera))))
    {
        fprintf(stderr, ERR_CAMERA_MALLOC);
        return NULL;
    }

    memset(cam, 0, sizeof(Camera));
    linkMethods(cam);

    glm_vec3_copy(position, cam->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, cam->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cam->up);
    glm_vec3_zero(cam->right);
    glm_vec3_copy(cam->up, cam->worldUp);

    cam->yaw = -90.0f;
    cam->pitch = 0.0f;

    cam->speed = 6.0f;
    cam->mouseSensitivity = 0.05f;
    cam->zoom = 45.0f;

    cam->attached = newList();

    cam->recordInitialPosition(cam);
    updateCameraVectors(cam);

    return cam;
}


static void linkMethods(Camera* this)
{
    this->getViewMatrix = getViewMatrix;

    this->moveForward = moveForward;
    this->moveLeft = moveLeft;
    this->moveBackward = moveBackward;
    this->moveRight = moveRight;
    this->moveMouse = moveMouse;
    this->scrollMouse = scrollMouse;

    this->attach = attach;
    this->detach = detach;

    this->setPosition = setPosition;
    this->setFront = setFront;
    this->setJump = setJump;

    this->recordInitialPosition = recordInitialPosition;
    this->resetPosition = resetPosition;
    this->resetFront = resetFront;

    this->poll = poll;
    this->destroy = destroy;
}


static void getViewMatrix(Camera* this, mat4 result)
{
    vec3 temp;
    glm_vec3_add(this->position, this->front, temp);
    glm_lookat(this->position, temp, this->up, result);
}


static void moveForward(Camera* this, float timeDelta)
{
    ListNode* node;
    Box* attach;

    vec3 temp;
    glm_vec3_copy((vec3){this->front[X_COORD], 0.0f, this->front[Z_COORD]}, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, this->speed * timeDelta, temp);
    glm_vec3_add(temp, this->position, this->position);

    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        attach->move(attach, temp);
    }
}


static void moveLeft(Camera* this, float timeDelta)
{
    ListNode* node;
    Box* attach;

    vec3 temp;
    glm_vec3_scale(this->right, this->speed * timeDelta, temp);
    glm_vec3_sub(this->position, temp, this->position);

    glm_vec3_negate(temp);

    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        attach->move(attach, temp);
    }
}


static void moveBackward(Camera* this, float timeDelta)
{
    ListNode* node;
    Box* attach;

    vec3 temp;
    glm_vec3_copy((vec3){this->front[X_COORD], 0.0f, this->front[Z_COORD]}, temp);
    glm_vec3_normalize(temp);
    glm_vec3_scale(temp, this->speed * timeDelta, temp);
    glm_vec3_sub(this->position, temp, this->position);

    glm_vec3_negate(temp);

    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        attach->move(attach, temp);
    }
}


static void moveRight(Camera* this, float timeDelta)
{
    ListNode* node;
    Box* attach;

    vec3 temp;
    glm_vec3_scale(this->right, this->speed * timeDelta, temp);
    glm_vec3_add(this->position, temp, this->position);

    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        attach->move(attach, temp);
    }
}


static void moveMouse(Camera* this, double xoffset,
                      double yoffset, bool constraint)
{
    ListNode* node;
    Box* attach;

    vec3 tempPos;
    vec3 temp;

    this->yaw += xoffset * this->mouseSensitivity;
    this->pitch += yoffset * this->mouseSensitivity;
    this->pitch = constraint && this->pitch > 89.0f ? 89.0f : this->pitch;
    this->pitch = constraint && this->pitch < -89.0f ? -89.0f : this->pitch;

    updateCameraVectors(this);

    glm_vec3_copy(this->front, temp);
    glm_vec3_normalize_to((vec3){temp[X_COORD], 0.0f, temp[Z_COORD]}, temp);
    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        glm_vec3_copy((vec3){this->position[X_COORD],
                             attach->position[Y_COORD],
                             this->position[Z_COORD]}, tempPos);
        glm_vec3_add(temp, tempPos, temp);
        attach->setPosition(attach, temp);
        attach->setRotation(attach, (vec3){0.0f, -(this->yaw - 90.0f), 0.0f});
    }
}


static void scrollMouse(Camera* this, float yoffset)
{
    this->zoom -= RANGE_INC(this->zoom, 1.0f, 45.0f) ? yoffset : 0;
    this->zoom = MAX(this->zoom, 1.0f);
    this->zoom = MIN(this->zoom, 45.0f);
}


static void attach(Camera* this, Box* box)
{
    this->attached->insertLast(this->attached, box, true);
}


static void detach(Camera* this)
{
    this->attached->deleteListShallow(&(this->attached));
    this->attached = newList();
}


static void setPosition(Camera* this, vec3 newPos)
{
    glm_vec3_copy(newPos, this->position);
}


static void setFront(Camera* this, vec3 newPos)
{
    glm_vec3_copy(newPos, this->front);
}


static void setJump(Camera* this, bool value)
{
    this->jumping = value;
}


static void recordInitialPosition(Camera* this)
{
    glm_vec3_copy(this->position, this->initialPosition);
}


static void resetPosition(Camera* this)
{
    ListNode* node;
    Box* attach;

    this->setPosition(this, this->initialPosition);

    LIST_FOR_EACH(this->attached, node)
    {
        attach = (Box*)(node->value);
        attach->resetPosition(attach);
        attach->resetRotation(attach);
    }
}


static void resetFront(Camera* this)
{
    this->setFront(this, (vec3){0.0f, 0.0f, -1.0f});
    this->yaw = -90.0f;
    this->pitch = 0.0f;
    updateCameraVectors(this);
}


static void poll(Camera* this)
{
    jump(this);
}


static void destroy(Camera* this)
{
    this->attached->deleteListShallow(&this->attached);
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


static void jump(Camera* this)
{
    static bool start = true;
    static float startTime = 0.0f;
    static float initialPosition = 0.0f;

    if (! this->jumping)
    {
        start = true;
        startTime = 0.0f;
        initialPosition = 0.0f;
        this->position[Y_COORD] = initialPosition;
        this->setJump(this, false);

        return;
    }

    if (start)
    {
        start = false;
        startTime = glfwGetTime();
        initialPosition = this->position[Y_COORD];
        return;
    }

    this->position[Y_COORD] = initialPosition + calcJump(glfwGetTime() - startTime);

    if ((this->position[Y_COORD] - initialPosition) < 0.0f)
    {
        start = true;
        startTime = 0.0f;
        initialPosition = 0.0f;
        this->position[Y_COORD] = initialPosition;
        this->setJump(this, false);
    }
}


static float calcJump(float t)
{
    return (JUMP_HEIGHT / _calcJump(JUMP_DURATION / 2.0f)) * _calcJump(t);
}


static float _calcJump(float t)
{
    return -((t - JUMP_DURATION) * t);
}
