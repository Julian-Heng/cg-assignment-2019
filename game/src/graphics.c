#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "camera.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

#include "graphics.h"


Backend* init()
{
    Backend* engine;

    if (! (engine = (Backend*)malloc(sizeof(Backend))))
    {
        fprintf(stderr, ERR_ENGINE_MALLOC);
        return NULL;
    }

    memset(engine, 0, sizeof(Backend));

    if (! (engine->cam = makeCamera()))
    {
        fprintf(stderr, ERR_CAMERA_MALLOC);
        return NULL;
    }

    initWindow(engine);
    initGlad(engine);

    if (engine->window)
    {
        glEnable(GL_DEPTH_TEST);
        initShader(engine);
        initShapes(engine);
        initTextures(engine);
    }

    glfwSetWindowUserPointer(engine->window, engine);

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
        glfwSetFramebufferSizeCallback(engine->window, framebufferSizeCallback);
        glfwSetKeyCallback(engine->window, normalInputCallback);
        glfwSetCursorPosCallback(engine->window, mouseCallback);
        glfwSetScrollCallback(engine->window, scrollCallback);
        glfwSetInputMode(engine->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        fprintf(stderr, ERR_WINDOW);
    }
}


void initGlad(Backend* engine)
{
    if (engine->window &&
        ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, ERR_GLAD);
        engine->window = NULL;
    }
}


void initShader(Backend* engine)
{
    Shader* shader = makeShader("shaders/shader.vs", "shaders/shader.fs");
    engine->shaderPrograms[engine->programCount++] = shader;
}


void initShapes(Backend* engine)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    vec3 cubePositions[] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        { 1.3f, -2.0f, -2.5f},
        { 1.5f,  2.0f, -2.5f},
        { 1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f}
    };

    engine->vertices = (float*)malloc(sizeof(vertices) * sizeof(float));
    engine->positions = (vec3*)malloc(sizeof(cubePositions) * sizeof(vec3));

    memcpy(engine->vertices, vertices, sizeof(vertices));
    memcpy(engine->positions, cubePositions, sizeof(cubePositions));

    glGenVertexArrays(1, &(engine->VAO));
    glGenBuffers(1, &(engine->VBO));

    glBindVertexArray(engine->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, engine->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
}


void loop(Backend* engine)
{
    float lastTime = glfwGetTime();
    float currentTime;

    Shader* shader = engine->shaderPrograms[0];

    shader->use(shader);
    shader->setInt(shader, "texture1", 0);
    shader->setInt(shader, "texture2", 1);

    while (! glfwWindowShouldClose(engine->window))
    {
        printFps(engine);
        keyInputCallback(engine->window);

        currentTime = glfwGetTime();
        engine->timeDelta = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(engine);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void printFps(Backend* engine)
{
    engine->frameDelta++;

    if ((glfwGetTime() - engine->fpsLastTime) >= 1.0)
    {
        fprintf(stderr, LOG_FPS, engine->frameDelta,
                                 1000.0 / (double)(engine->frameDelta));
        engine->frameDelta = 0;
        engine->fpsLastTime += 1.0f;
    }
}


void draw(Backend* engine)
{
    Shader* shader = engine->shaderPrograms[0];
    int width, height;
    int i;

    mat4 model;
    mat4 projection;
    mat4 view;

    glfwGetWindowSize(engine->window, &width, &height);

    glm_mat4_identity(model);
    glm_mat4_identity(projection);
    glm_mat4_identity(view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, engine->textures[0]);

    shader->use(shader);

    glm_perspective(glm_rad(engine->cam->zoom), ASPECT_RATIO(width, height),
                    0.1f, 100.0f, projection);
    engine->cam->getViewMatrix(engine->cam, view);

    shader->setMat4(shader, "projection", projection);
    shader->setMat4(shader, "view", view);

    glBindVertexArray(engine->VAO);

    for (i = 0; i < 10; i++)
    {
        glm_mat4_identity(model);

        glm_translate(model, engine->positions[i]);
        glm_rotate(model, glm_rad(20.0f * i), (vec3){1.0f, 0.3f, 0.5f});
        glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});

        shader->setMat4(shader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}


void toggleWireframe()
{
    static int toggle = 0;
    toggle = (toggle + 1) % 3;
    glPolygonMode(GL_FRONT_AND_BACK, toggle == 0 ? GL_FILL :
                                     toggle == 1 ? GL_LINE :
                                     GL_POINT);
}


void normalInputCallback(GLFWwindow* win, int key, int scancode,
                         int action, int mods)
{
    if (action != GLFW_PRESS)
    {
        return;
    }

    switch (key)
    {
        case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
            glfwSetWindowShouldClose(win, true);
            break;

        case GLFW_KEY_TAB:
            toggleWireframe();
            break;
    }
}


void keyInputCallback(GLFWwindow* win)
{
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);
    Camera* cam = engine->cam;
    float timeDelta = engine->timeDelta;

    int i, n;
    int pressed[] = {
        glfwGetKey(win, GLFW_KEY_W),
        glfwGetKey(win, GLFW_KEY_A),
        glfwGetKey(win, GLFW_KEY_S),
        glfwGetKey(win, GLFW_KEY_D)
    };

    void (*camActions[])(Camera*, float) = {
        cam->moveForward,
        cam->moveLeft,
        cam->moveBackward,
        cam->moveRight
    };

    n = sizeof(pressed) / sizeof(int);

    for (i = 0; i < n; i++)
    {
        if (pressed[i])
            camActions[i](cam, timeDelta);
    }
}


void mouseCallback(GLFWwindow* win, double x, double y)
{
    static bool firstMouse = true;
    static double lastX = 0.0f;
    static double lastY = 0.0f;

    Camera* cam = ((Backend*)glfwGetWindowUserPointer(win))->cam;

    float xoffset;
    float yoffset;

    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    xoffset = x - lastX;
    yoffset = lastY - y;

    lastX = x;
    lastY = y;

    cam->moveMouse(cam, xoffset, yoffset, true);
}


void scrollCallback(GLFWwindow* win, double xoffset, double yoffset)
{
    Camera* cam = ((Backend*)glfwGetWindowUserPointer(win))->cam;
    cam->scrollMouse(cam, yoffset);
}


void framebufferSizeCallback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
    draw((Backend*)glfwGetWindowUserPointer(win));
}


void terminate(Backend** engine)
{
    int i;

    glDeleteVertexArrays(1, &((*engine)->VAO));
    glDeleteBuffers(1, &((*engine)->VBO));

    if (*engine)
    {
        for (i = 0; i < (*engine)->programCount; i++)
        {
            free((*engine)->shaderPrograms[i]);
            (*engine)->shaderPrograms[i] = NULL;
        }

        SAFE_FREE((*engine)->vertices);
        SAFE_FREE((*engine)->positions);

        free(*engine);
        *engine = NULL;
    }

    glfwTerminate();
}
