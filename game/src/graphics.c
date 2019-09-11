#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "shader.h"
#include "graphics.h"

#define WIDTH 1440
#define HEIGHT 900
#define TITLE "Game"

backend* init()
{
    backend* engine;

    if ((engine = (backend*)malloc(sizeof(backend))))
    {
        memset(engine, 0, sizeof(backend));

        initWindow(engine);
        initGlad(engine);

        if (engine->window)
        {
            initShader(engine);
            initShapes(engine);
        }
    }
    else
    {
        fprintf(stderr, "Cannot allocate memory\n");
    }

    return engine;
}


void initWindow(backend* engine)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__) && defined(__MACH__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if ((engine->window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)))
    {
        glfwMakeContextCurrent(engine->window);
        glfwSetKeyCallback(engine->window, input);
        glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);
    }
    else
    {
        fprintf(stderr, "Failed to initialise window\n");
    }
}


void initGlad(backend* engine)
{
    if (engine->window &&
        ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialise GLAD\n");
        engine->window = NULL;
    }
}


void initShader(backend* engine)
{
    int shader = makeShader("shader.vs", "shader.fs");
    engine->shaderPrograms[engine->programCount++] = shader;
}


void initShapes(backend* engine)
{
    float vertices[] = {
        // Positions            Color
        0.5f,   0.5f,   0.0f,   1.0f, 0.0f, 0.0f,   // top right
        0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   // bottom right
        -0.5f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   // bottom left
        -0.5f,  0.5f,   0.0f,   1.0f, 0.0f, 0.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &(engine->VAO));
    glGenBuffers(1, &(engine->VBO));
    glGenBuffers(1, &(engine->EBO));

    glBindVertexArray(engine->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, engine->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void loop(backend* engine)
{
    float timeValue;
    float greenValue;
    int vertexColorLocation;

    while (! glfwWindowShouldClose(engine->window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        useShader(engine->shaderPrograms[0]);

        timeValue = glfwGetTime();
        greenValue = sin(timeValue) / 2.0f + 0.5f;
        vertexColorLocation = glGetUniformLocation(engine->shaderPrograms[0], "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(engine->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void terminate(backend** engine)
{
    glDeleteVertexArrays(1, &((*engine)->VAO));
    glDeleteBuffers(1, &((*engine)->VBO));

    if (*engine)
    {
        free(*engine);
        *engine = NULL;
    }

    glfwTerminate();
}


void input(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    static int wireframeToggle = 0;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
                glfwSetWindowShouldClose(win, true);
                break;
            case GLFW_KEY_TAB:
                wireframeToggle= (wireframeToggle + 1) % 3;
                glPolygonMode(GL_FRONT_AND_BACK,
                              wireframeToggle == 0 ? GL_FILL :
                              wireframeToggle == 1 ? GL_LINE :
                              GL_POINT);
                break;
            default:
                break;
        }
    }
}


void framebuffer_size_callback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
}
