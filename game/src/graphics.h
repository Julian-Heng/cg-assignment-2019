#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>

typedef struct Backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int shaderPrograms[BUFSIZ];
    unsigned int programCount;

    unsigned int textures[BUFSIZ];
    unsigned int textureCount;
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
