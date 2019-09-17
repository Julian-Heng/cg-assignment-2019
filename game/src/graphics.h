#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>

#include "list.h"
#include "camera.h"
#include "shader.h"

#define WIDTH 1440
#define HEIGHT 900
#define TITLE "CG Assignment"

#define LOG_FPS "%d fps, %0.5f ms\n"
#define ERR_ENGINE_MALLOC "Cannot allocate memory for engine\n"
#define ERR_CAMERA_MALLOC "Cannot allocate memory for camera\n"
#define ERR_WINDOW "Failed to initialise window\n"
#define ERR_GLAD "Failed to initialise GLAD\n"


typedef struct Backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;

    unsigned int frameDelta;
    double fpsLastTime;

    Shader* shaderPrograms[BUFSIZ];
    unsigned int programCount;

    unsigned int textures[BUFSIZ];
    unsigned int textureCount;

    /*
    float* vertices;
    vec3* positions;
    */
    List* boxes;

    Camera* cam;
    float timeDelta;
} Backend;

Backend* init(void);
void initWindow(Backend*);
void initGlad(Backend*);
void initShader(Backend*);
void initShapes(Backend*);
void initTextures(Backend*);

void loop(Backend*);

void printFps(Backend* engine);
void draw(Backend* engine);

void toggleWireframe(void);

void normalInputCallback(GLFWwindow*, int, int, int, int);
void keyInputCallback(GLFWwindow*);
void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void framebufferSizeCallback(GLFWwindow*, int, int);

void terminate(Backend**);


#endif
