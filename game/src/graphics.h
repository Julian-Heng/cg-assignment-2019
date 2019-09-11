#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct backend
{
    GLFWwindow* window;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    int shaderProgram;
} backend;

backend* init(void);
void initWindow(backend*);
void initGlad(backend*);
void initShader(backend*);
void initShapes(backend*);
void loop(backend*);
void terminate(backend**);
void framebuffer_size_callback(GLFWwindow*, int, int);

#endif
