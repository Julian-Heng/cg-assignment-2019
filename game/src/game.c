#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>

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
