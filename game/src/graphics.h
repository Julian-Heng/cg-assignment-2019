#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cglm/cglm.h>
#include <stdio.h>
#include "shader.h"

#define WIDTH 800
#define HEIGHT 600
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)
#define TITLE "CG Assignment"

#define LOG_FPS "%d fps, %0.5f ms\n"
#define ERR_WINDOW "Failed to initialise window\n"
#define ERR_GLAD "Failed to initialise GLAD\n"

typedef struct Backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;

    Shader* shaderPrograms[BUFSIZ];
    unsigned int programCount;

    unsigned int textures[BUFSIZ];
    unsigned int textureCount;

    float* vertices;
    vec3* positions;
} Backend;

Backend* init(void);
void initWindow(Backend*);
void initGlad(Backend*);
void initShader(Backend*);
void initShapes(Backend*);
void initTextures(Backend*);


void loop(Backend*);

void terminate(Backend**);

void input(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow*, int, int);

#endif
