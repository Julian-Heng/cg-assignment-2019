#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

int main()
{
    GLFWwindow* win;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__) && defined(__MACH__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if ((win = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game", NULL, NULL)) != NULL)
    {
        glfwMakeContextCurrent(win);
        glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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

            glfwTerminate();
        }
        else
        {
            fprintf(stderr, "Failed to initialise GLAD\n");
        }
    }
    else
    {
        fprintf(stderr, "Failed to initialise window\n");
    }
}

void framebuffer_size_callback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
}
