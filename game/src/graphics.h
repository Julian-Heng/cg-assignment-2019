#ifndef GRAPHICS_H
#define GRAPHICS_H

GLFWwindow* init(void);
void windowInit(GLFWwindow**);
void gladInit(GLFWwindow**);
void terminate(void);
void loop(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);

#endif
