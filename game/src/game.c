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
#include "log.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

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
    List* shaders = newList();

    shaders->insertLast(
        shaders,
        newShader("shaders/shader.vs", "shaders/shader.fs"),
        true
    );

    shaders->insertLast(
        shaders,
        newShader("shaders/lamp.vs", "shaders/lamp.fs"),
        true
    );

    engine->shaders = shaders;
}


void initShapes(Backend* engine)
{
    Box* lamp;

    int i;
    vec3 boxPositions[] = {
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

    vec3 lampPositions[] = {
        { 1.2f, 1.0f,  2.0f}
    };

    List* boxes = newList();
    List* lamps = newList();

    for (i = 0; i < sizeof(boxPositions) / sizeof(vec3); i++)
    {
        boxes->insertLast(boxes, newBox(boxPositions[i]), true);
    }

    for (i = 0; i < sizeof(lampPositions) / sizeof(vec3); i++)
    {
        lamp = newBox(lampPositions[i]);
        lamp->setScale(lamp, (vec3){0.1f, 0.1f, 0.1f});
        lamps->insertLast(lamps, lamp, true);
    }

    engine->boxes = boxes;
    engine->lamps = lamps;
}


void initTextures(Backend* engine)
{
    List* textures = newList();

    Box* box;
    ListNode* iter1;
    ListNode* iter2;

    Texture* texture;

    textures->insertLast(textures,
                         newTexture("resources/container2.png", GL_RGBA, false),
                         true);

    textures->insertLast(textures,
                         newTexture("resources/container2_specular.png", GL_RGBA, false),
                         true);

    FOR_EACH(engine->boxes, iter1)
    {
        box = (Box*)iter1->value;

        FOR_EACH(textures, iter2)
        {
            texture = (Texture*)iter2->value;
            box->addTexture(box, texture);
        }
    }

    engine->textures = textures;
}


void loop(Backend* engine)
{
    float lastTime = glfwGetTime();
    float currentTime;

    Shader* shader;
    engine->shaders->peekFirst(engine->shaders, (void**)&shader, NULL);
    shader->use(shader);

    while (! glfwWindowShouldClose(engine->window))
    {
        logInfo(stderr, engine);
        instantKeyInputCallback(engine->window);

        currentTime = glfwGetTime();
        engine->timeDelta = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(engine);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void draw(Backend* engine)
{
    Shader* normalShader;
    Shader* lampShader;

    Box* box;
    Camera* cam;

    mat4 projection;
    mat4 view;

    ListNode* node;

    cam = engine->cam;

    engine->shaders->peekAt(engine->shaders, 0, (void**)&normalShader, NULL);
    engine->shaders->peekAt(engine->shaders, 1, (void**)&lampShader, NULL);

    glfwGetWindowSize(engine->window, &(engine->width), &(engine->height));

    glm_mat4_identity(projection);
    glm_mat4_identity(view);

    normalShader->use(normalShader);
    glm_perspective(glm_rad(cam->zoom),
                    ASPECT_RATIO(engine->width, engine->height),
                    0.1f, 100.0f, projection);
    cam->getViewMatrix(cam, view);

    normalShader->setMat4(normalShader, "projection", projection);
    normalShader->setMat4(normalShader, "view", view);

    normalShader->setVec3(normalShader, "objectColor", (vec3){1.0f, 0.5f, 0.31f});
    normalShader->setVec3(normalShader, "lightColor", (vec3){1.0f, 1.0f, 1.0f});
    normalShader->setVec3(normalShader, "viewPos", cam->position);

    normalShader->setVec3(normalShader, "material.ambient", (vec3){1.0f, 0.5f, 0.31f});
    normalShader->setVec3(normalShader, "material.diffuse", (vec3){1.0f, 0.5f, 0.31f});
    normalShader->setVec3(normalShader, "material.specular", (vec3){0.5f, 0.5f, 0.5f});
    normalShader->setFloat(normalShader, "material.shininess", 32.0f);

    normalShader->setVec3(normalShader, "light.ambient", (vec3){0.2f, 0.2f, 0.2f});
    normalShader->setVec3(normalShader, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f});
    normalShader->setVec3(normalShader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});

    normalShader->setFloat(normalShader, "light.constant", 1.0f);
    normalShader->setFloat(normalShader, "light.linear", 0.09f);
    normalShader->setFloat(normalShader, "light.quadratic", 0.032f);

    normalShader->setInt(normalShader, "material.diffuse", 0);
    normalShader->setInt(normalShader, "material.specular", 1);

    engine->lamps->peekFirst(engine->lamps, (void**)&box, NULL);
    normalShader->setVec3(normalShader, "lightPos", box->position);

    FOR_EACH(engine->boxes, node)
    {
        box = (Box*)node->value;
        box->setShader(box, normalShader);
        box->draw(box);
    }

    lampShader->use(lampShader);

    lampShader->setMat4(lampShader, "projection", projection);
    lampShader->setMat4(lampShader, "view", view);

    FOR_EACH(engine->lamps, node)
    {
        box = (Box*)node->value;
        box->setShader(box, lampShader);
        box->draw(box);
    }

    cam->poll(cam);
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


void instantKeyInputCallback(GLFWwindow* win)
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
    static bool first = true;
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

    if (first)
    {
        lastX = x;
        lastY = y;
        first = false;
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

    FOR_EACH(_engine->boxes, iter)
    {
        box = (Box*)iter->value;
        box->textures->deleteListShallow(&(box->textures));
    }

    _engine->shaders->deleteList(&(_engine->shaders));
    _engine->textures->deleteList(&(_engine->textures));
    _engine->boxes->deleteList(&(_engine->boxes));
    _engine->lamps->deleteList(&(_engine->lamps));

    free(_engine);
    _engine = NULL;

    glfwTerminate();
}
