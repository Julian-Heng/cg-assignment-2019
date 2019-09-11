#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "graphics.h"
#include "game.h"

int main()
{
    GLFWwindow* win = init();

    if (win)
    {
        loop(win);
    }

    terminate();
    return win == NULL;
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
