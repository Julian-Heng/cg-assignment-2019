#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "shader.h"
#include "texture.h"
#include "graphics.h"

#define WIDTH 800
#define HEIGHT 600
#define TITLE "Game"

#define LOG_FPS "%d fps, %0.5f ms\n"

Backend* init()
{
    Backend* engine;

    if ((engine = (Backend*)malloc(sizeof(Backend))))
    {
        memset(engine, 0, sizeof(Backend));

        initWindow(engine);
        initGlad(engine);

        if (engine->window)
        {
            initShader(engine);
            initShapes(engine);
            initTextures(engine);
        }
    }
    else
    {
        fprintf(stderr, "Cannot allocate memory\n");
    }

    return engine;
}


void initWindow(Backend* engine)
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


void initGlad(Backend* engine)
{
    if (engine->window &&
        ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialise GLAD\n");
        engine->window = NULL;
    }
}


void initShader(Backend* engine)
{
    int shader = makeShader("shaders/shader.vs", "shaders/shader.fs");
    engine->shaderPrograms[engine->programCount++] = shader;
}


void initShapes(Backend* engine)
{
    float vertices[] = {
        // Positions            Texture
        0.5f,   0.5f,   0.0f,   1.0f,   1.0f,   // top right
        0.5f,   -0.5f,  0.0f,   1.0f,   0.0f,   // bottom right
        -0.5f,  -0.5f,  0.0f,   0.0f,   0.0f,   // bottom left
        -0.5f,  0.5f,   0.0f,   0.0f,   1.0f    // top left
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void initTextures(Backend* engine)
{
    TextureSpec spec;

    spec.filename = "resources/container.jpg";
    spec.rgbMode = GL_RGB;
    spec.flip = false;
    generateTexture(&(engine->textures[engine->textureCount++]), spec);

    spec.filename = "resources/awesomeface.png";
    spec.rgbMode = GL_RGBA;
    spec.flip = true;
    generateTexture(&(engine->textures[engine->textureCount++]), spec);
}


void loop(Backend* engine)
{
    double lastTime = glfwGetTime();
    double currentTime;
    unsigned int nFrames = 0;

    unsigned int transformLoc;
    useShader(engine->shaderPrograms[0]);
    setShaderInt(engine->shaderPrograms[0], "texture1", 0);
    setShaderInt(engine->shaderPrograms[0], "texture2", 1);

    while (! glfwWindowShouldClose(engine->window))
    {
        currentTime = glfwGetTime();
        nFrames++;

        if (currentTime - lastTime >= 1.0)
        {
            fprintf(stderr, LOG_FPS, nFrames, 1000.0 / (double)nFrames);
            nFrames = 0;
            lastTime += 1.0;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, engine->textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, engine->textures[1]);

        mat4 transform = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        };

        glm_rotate(transform, (float)glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});

        useShader(engine->shaderPrograms[0]);
        transformLoc = glGetUniformLocation(engine->shaderPrograms[0], "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform[0]);

        glBindVertexArray(engine->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void terminate(Backend** engine)
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
