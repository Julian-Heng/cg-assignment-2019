#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "box.h"
#include "camera.h"
#include "list.h"
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

    if (! (engine->cam = newCamera()))
    {
        free(engine);
        engine = NULL;
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

    if (! (engine->window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)))
    {
        fprintf(stderr, ERR_WINDOW);
    }
    else
    {
        glfwMakeContextCurrent(engine->window);

        glfwSetCursorPosCallback(engine->window, mouseCallback);
        glfwSetFramebufferSizeCallback(engine->window, framebufferSizeCallback);
        glfwSetKeyCallback(engine->window, normalInputCallback);
        glfwSetScrollCallback(engine->window, scrollCallback);

        glfwSetInputMode(engine->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}


void initGlad(Backend* engine)
{
    if (engine->window && ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, ERR_GLAD);
        engine->window = NULL;
    }
}


void initShader(Backend* engine)
{
    engine->shaders = newList();

    engine->shaders->insertLast(
        engine->shaders,
        newShader("shaders/shader.vs", "shaders/shader.fs"),
        true
    );
}


void initShapes(Backend* engine)
{
    int i;
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

    engine->boxes = newList();
    engine->lamps = newList();

    for (i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++)
    {
        engine->boxes->insertLast(engine->boxes, newBox(cubePositions[i]), true);
    }
}


void initTextures(Backend* engine)
{
    Box* box;
    ListNode* iter1;
    ListNode* iter2;

    Texture* texture;
    engine->textures = newList();

    texture = newTexture("resources/container.jpg", GL_RGB, false);
    engine->textures->insertLast(engine->textures, texture, true);

    iter1 = engine->boxes->head;
    while (iter1)
    {
        box = (Box*)iter1->value;

        iter2 = engine->textures->head;
        while (iter2)
        {
            texture = (Texture*)iter2->value;
            box->addTexture(box, texture);
            iter2 = iter2->next;
        }

        iter1 = iter1->next;
    }
}


void loop(Backend* engine)
{
    float lastTime = glfwGetTime();
    float currentTime;

    Shader* shader;
    Camera* cam;

    engine->shaders->peekFirst(engine->shaders, (void**)&shader, NULL);
    cam = engine->cam;

    shader->use(shader);
    shader->setInt(shader, "texture1", 0);
    shader->setInt(shader, "texture2", 1);

    while (! glfwWindowShouldClose(engine->window))
    {
        printInfo(engine);
        keyInputCallback(engine->window);

        currentTime = glfwGetTime();
        engine->timeDelta = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam->poll(cam);
        draw(engine);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void printInfo(Backend* engine)
{
    static unsigned int freezeFrameDelta;
    static float freezeFrameLatency;

    int rows = 0;
    engine->frameDelta++;

    if ((glfwGetTime() - engine->fpsLastTime) >= 1.0)
    {
        freezeFrameDelta = engine->frameDelta;
        freezeFrameLatency = 1000.0 / (double)(freezeFrameDelta);

        engine->frameDelta = 0;
        engine->fpsLastTime += 1.0f;
    }

    fprintf(stderr, LOG_CLEAR LOG_FPS "\n", freezeFrameDelta);
    rows++;

    fprintf(stderr, LOG_CLEAR LOG_FRAME_LATENCY "\n", freezeFrameLatency);
    rows++;

    fprintf(stderr, LOG_CLEAR LOG_CAM_LOCATION "\n", engine->cam->position[0],
                                                     engine->cam->position[1],
                                                     engine->cam->position[2]);
    rows++;

    fprintf(stderr, LOG_CLEAR LOG_CAM_FRONT "\n", engine->cam->front[0],
                                                  engine->cam->front[1],
                                                  engine->cam->front[2]);
    rows++;
    fprintf(stderr, "\e[%dA", rows);
}


void draw(Backend* engine)
{
    Shader* shader;
    Texture* texture;
    Box* box;

    int width, height;

    mat4 projection;
    mat4 view;

    ListNode* node;

    engine->shaders->peekFirst(engine->shaders, (void**)&shader, NULL);
    engine->textures->peekFirst(engine->textures, (void**)&texture, NULL);
    glfwGetWindowSize(engine->window, &width, &height);

    glm_mat4_identity(projection);
    glm_mat4_identity(view);

    shader->use(shader);
    glm_perspective(glm_rad(engine->cam->zoom), ASPECT_RATIO(width, height),
                    0.1f, 100.0f, projection);
    engine->cam->getViewMatrix(engine->cam, view);

    shader->setMat4(shader, "projection", projection);
    shader->setMat4(shader, "view", view);

    node = engine->boxes->head;

    while (node)
    {
        box = (Box*)node->value;
        box->setShader(box, shader);
        box->draw(box);
        node = node->next;
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
    Camera* cam = engine ? engine->cam : NULL;

    if (! cam)
    {
        return;
    }

    float timeDelta = engine->timeDelta;

    bool keys[] = {
        KEY_PRESSED(win, GLFW_KEY_W),
        KEY_PRESSED(win, GLFW_KEY_A),
        KEY_PRESSED(win, GLFW_KEY_S),
        KEY_PRESSED(win, GLFW_KEY_D),
        KEY_PRESSED(win, GLFW_KEY_SPACE),

        KEY_PRESSED(win, GLFW_KEY_R)
    };

    if (keys[CAM_MOVE_FORWARD] && ! keys[CAM_MOVE_BACKWARD])
    {
        cam->moveForward(cam, timeDelta);
    }

    if (keys[CAM_MOVE_LEFT] && ! keys[CAM_MOVE_RIGHT])
    {
        cam->moveLeft(cam, timeDelta);
    }

    if (keys[CAM_MOVE_BACKWARD] && ! keys[CAM_MOVE_FORWARD])
    {
        cam->moveBackward(cam, timeDelta);
    }

    if (keys[CAM_MOVE_RIGHT] && ! keys[CAM_MOVE_LEFT])
    {
        cam->moveRight(cam, timeDelta);
    }

    if (keys[CAM_JUMP])
    {
        cam->setJump(cam, true);
    }

    if (keys[CAM_RESET])
    {
        cam->resetPosition(cam);
        cam->resetFront(cam);
    }
}


void mouseCallback(GLFWwindow* win, double x, double y)
{
    static bool firstMouse = true;
    static double lastX = 0.0f;
    static double lastY = 0.0f;

    float xoffset;
    float yoffset;

    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);
    Camera* cam = engine ? engine->cam : NULL;

    if (! cam)
    {
        return;
    }

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
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);
    Camera* cam = engine ? engine->cam : NULL;

    if (cam)
    {
        cam->scrollMouse(cam, yoffset);
    }
}


void framebufferSizeCallback(GLFWwindow* win, int width, int height)
{
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);
    glViewport(0, 0, width, height);
    draw(engine);
}


void terminate(Backend** engine)
{
    Backend* _engine = *engine;
    Box* box;
    ListNode* iter;
    glDeleteVertexArrays(1, &(_engine->VAO));
    glDeleteBuffers(1, &(_engine->VBO));

    if (! _engine)
    {
        return;
    }

    iter = _engine->boxes->head;

    while (iter)
    {
        box = (Box*)iter->value;
        box->textures->deleteListShallow(&(box->textures));
        iter = iter->next;
    }

    _engine->shaders->deleteList(&(_engine->shaders));
    _engine->textures->deleteList(&(_engine->textures));
    _engine->boxes->deleteList(&(_engine->boxes));
    _engine->lamps->deleteList(&(_engine->lamps));

    free(_engine);
    _engine = NULL;

    glfwTerminate();
}
