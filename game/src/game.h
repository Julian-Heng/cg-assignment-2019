#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cglm/vec3.h>
#include <stdio.h>

#include "camera.h"
#include "hashtable.h"
#include "list.h"
#include "shader.h"

#define WIDTH 1440
#define HEIGHT 900
#define TITLE "CG Assignment"

#define ERR_ENGINE_MALLOC "Error: Unable to allocate memory for engine\n"
#define ERR_WINDOW "Error: failed to initialise window\n"
#define ERR_GLAD "Error: failed to initialise GLAD\n"


typedef enum
{
    CAM_MOVE_FORWARD,
    CAM_MOVE_LEFT,
    CAM_MOVE_BACKWARD,
    CAM_MOVE_RIGHT,
    CAM_JUMP,

    GAME_RESET
} KeyAction;


typedef enum
{
    GAME_USE_PERSPECTIVE,
    GAME_LIGHTS_ON,
    GAME_HAS_TORCH,

    GAME_PICKUP_WOLF,
    GAME_PLAYER_DIE,
    GAME_WIN,

    GAME_OPTION_COUNT
} GameOptions;


typedef struct Backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;

    bool options[GAME_OPTION_COUNT];

    vec3 safeZone;
    Camera* cam;
    float timeDelta;

    int width;
    int height;

    float lightLevel;

    HashTable* textures;
    HashTable* shaders;
    HashTable* models;
} Backend;


Backend* init(void);
void initWindow(Backend*);
void initGlad(Backend*);
void initShader(Backend*);
void initTextures(Backend*);
void initShapes(Backend*);
void resetGameSettings(Backend*);

void loop(Backend*);
void draw(Backend*);
void drawMessage(Backend*, const char*);

void drawWolfTail(Box*, mat4, void*);
void drawSheepLeg(Box*, mat4, void*);
bool checkHitbox(Backend*, vec3, float);

void setupProjection(Backend*, Camera*, mat4);
void setupShader(Backend*, Shader*, Camera*, mat4, mat4);
void toggleWireframe(void);
void normalInputCallback(GLFWwindow*, int, int, int, int);
void instantKeyInputCallback(GLFWwindow*);
void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void framebufferSizeCallback(GLFWwindow*, int, int);

void terminate(Backend**);


#endif
