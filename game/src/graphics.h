#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>

typedef struct backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    int shaderPrograms[BUFSIZ];
    unsigned int programCount;

    unsigned int texture;
} backend;

backend* init(void);
void initWindow(backend*);
void initGlad(backend*);
void initShader(backend*);
void initShapes(backend*);


void loop(backend*);

void terminate(backend**);

void input(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow*, int, int);

#endif
