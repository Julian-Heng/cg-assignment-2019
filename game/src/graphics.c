#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include "graphics.h"

#define WIDTH 1440
#define HEIGHT 900
#define TITLE "Game"

GLFWwindow* init()
{
    GLFWwindow* win;
    windowInit(&win);
    gladInit(&win);
    return win;
}

void windowInit(GLFWwindow** win)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__) && defined(__MACH__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (! (*win = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)))
    {
        fprintf(stderr, "Failed to initialise window\n");
    }
}

void gladInit(GLFWwindow** win)
{
    if (*win)
    {
        glfwMakeContextCurrent(*win);
        glfwSetFramebufferSizeCallback(*win, framebuffer_size_callback);

        if (! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            fprintf(stderr, "Failed to initialise GLAD\n");
            *win = NULL;
        }
    }
}

void terminate()
{
    glfwTerminate();
}

void loop(GLFWwindow* win)
{
    while (! glfwWindowShouldClose(win))
    {
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}

void framebuffer_size_callback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
}
