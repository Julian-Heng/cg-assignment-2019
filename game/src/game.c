#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdbool.h>

#include "graphics.h"
#include "game.h"

int main()
{
    bool ret = false;
    Backend* engine = init();

    if (engine->window)
    {
        loop(engine);
    }

    terminate(&engine);
    return (int)ret;
}
