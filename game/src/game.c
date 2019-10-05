#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <cglm/io.h>

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
        initTextures(engine);
        initShapes(engine);
    }

    glfwSetWindowUserPointer(engine->window, engine);

    engine->usePerspective = true;
    engine->lightsOn = false;

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


void initTextures(Backend* engine)
{
    List* textures = newList();

    textures->insertLast(
        textures,
        newTexture("resources/container2.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/container2_specular.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/grass.png", GL_RGBA, false),
        true
    );

    engine->textures = textures;
}


void initShapes(Backend* engine)
{
    Texture* texture;

    Box* box;
    int i, j;

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
        {-1.3f,  1.0f, -1.5f},
        { 0.5f, -0.5f,  1.5f}
    };

    List* boxes = newList();

    for (i = 0; i < sizeof(boxPositions) / sizeof(vec3); i++)
    {
        box = newBox(boxPositions[i]);

        box->material->setAmbient(box->material, (vec3){1.0f, 0.5f, 0.31f});
        box->material->setDiffuse(box->material, 0);
        box->material->setSpecular(box->material, 1);
        box->material->setShininess(box->material, 32.0f);

        engine->textures->peekAt(engine->textures, 0, (void**)&texture, NULL);
        box->addTexture(box, texture);
        engine->textures->peekAt(engine->textures, 1, (void**)&texture, NULL);
        box->addTexture(box, texture);

        boxes->insertLast(boxes, box, true);
    }

    boxes->peekLast(boxes, (void**)&box, NULL);
    box->setScale(box, (vec3){0.5f, 0.5f, 0.5f});
    engine->cam->attach(engine->cam, box);

    engine->textures->peekAt(engine->textures, 2, (void**)&texture, NULL);

    for (i = -50; i < 50; i += 5)
    {
        for (j = -50; j < 50; j += 5)
        {
            box = newBox((vec3){(float)i, -2.0f, (float)j});

            box->material->setAmbient(box->material, (vec3){1.0f, 0.5f, 0.31f});
            box->material->setDiffuse(box->material, 0);
            box->material->setSpecular(box->material, 2);
            box->material->setShininess(box->material, 32.0f);

            box->setScale(box, (vec3){5.0f, 2.0f, 5.0f});
            box->addTexture(box, texture);
            boxes->insertLast(boxes, box, true);
        }
    }

    engine->boxes = boxes;
}


void loop(Backend* engine)
{
    float lastTime = glfwGetTime();
    float currentTime;

    while (! glfwWindowShouldClose(engine->window))
    {
        logInfo(stderr, engine);
        instantKeyInputCallback(engine->window);

        currentTime = glfwGetTime();
        engine->timeDelta = currentTime - lastTime;
        lastTime = currentTime;

        if (! engine->lightsOn)
        {
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        }
        else
        {
            glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(engine);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void draw(Backend* engine)
{
    Shader* normalShader;

    Box* box;
    Camera* cam;

    mat4 projection;
    mat4 view;

    ListNode* node;

    cam = engine->cam;

    engine->shaders->peekAt(engine->shaders, 0, (void**)&normalShader, NULL);

    glfwGetWindowSize(engine->window, &(engine->width), &(engine->height));

    glm_mat4_identity(projection);
    glm_mat4_identity(view);

    normalShader->use(normalShader);

    cam->getViewMatrix(cam, view);

    if (engine->usePerspective)
    {
        glm_perspective(glm_rad(cam->zoom),
                        ASPECT_RATIO(engine->width, engine->height),
                        0.1f, 100.0f, projection);
    }
    else
    {
        glm_ortho(-((float)engine->width / 400.0f),
                   (float)engine->width / 400.0f,
                  -((float)engine->height / 400.0f),
                   (float)engine->height / 400.0f,
                  -1000.0f, 1000.0f, projection);
    }

    normalShader->setMat4(normalShader, "projection", projection);
    normalShader->setMat4(normalShader, "view", view);
    normalShader->setVec3(normalShader, "viewPos", cam->position);

    normalShader->setBool(normalShader, "lightsOn", engine->lightsOn);

    if (! engine->lightsOn)
    {
        normalShader->setVec3(normalShader, "light.ambient", (vec3){0.2f, 0.2f, 0.2f});
        normalShader->setVec3(normalShader, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f});
        normalShader->setVec3(normalShader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});
    }
    else
    {
        normalShader->setVec3(normalShader, "light.ambient", (vec3){1.0f, 1.0f, 1.0f});
        normalShader->setVec3(normalShader, "light.diffuse", (vec3){1.0f, 1.0f, 1.0f});
        normalShader->setVec3(normalShader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});
    }

    normalShader->setVec3(normalShader, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f});
    normalShader->setVec3(normalShader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});

    normalShader->setFloat(normalShader, "light.constant", 1.0f);
    normalShader->setFloat(normalShader, "light.linear", 0.09f);
    normalShader->setFloat(normalShader, "light.quadratic", 0.032f);

    normalShader->setVec3(normalShader, "light.position", cam->position);
    normalShader->setVec3(normalShader, "light.direction", cam->front);
    normalShader->setFloat(normalShader, "light.cutOff", cos(glm_rad(17.5f)));
    normalShader->setFloat(normalShader, "light.outerCutOff", cos(glm_rad(35.0f)));

    FOR_EACH(engine->boxes, node)
    {
        box = (Box*)node->value;
        box->setShader(box, normalShader);
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
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);

    if (action != GLFW_PRESS || ! engine)
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

        case GLFW_KEY_P:
            engine->usePerspective = ! engine->usePerspective;
            break;

        case GLFW_KEY_O:
            engine->lightsOn = ! engine->lightsOn;
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
        cam->setJump(cam, false);
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

    _engine->textures->deleteList(&(_engine->textures));
    _engine->shaders->deleteList(&(_engine->shaders));
    _engine->boxes->deleteList(&(_engine->boxes));

    _engine->cam->destroy(_engine->cam);

    free(_engine);
    _engine = NULL;

    glfwTerminate();
}
