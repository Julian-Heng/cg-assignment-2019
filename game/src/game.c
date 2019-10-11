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


int main(void)
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
        newTexture("resources/grass.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/tree_1.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/tree_2.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/grey.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/wolf_face.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/black.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/sheep_skin.png", GL_RGBA, false),
        true
    );

    textures->insertLast(
        textures,
        newTexture("resources/sheep_face.png", GL_RGBA, false),
        true
    );

    engine->textures = textures;
}


void initShapes(Backend* engine)
{
    Texture* texture;

    Box* root;
    Box* box;
    int i, j;

    List* boxes = newList();

    // Ground
    engine->textures->peekAt(engine->textures, 0, (void**)&texture, NULL);

    for (i = -50; i < 50; i += 10)
    {
        for (j = -50; j < 50; j += 10)
        {
            box = newBox((vec3){(float)i, -2.0f, (float)j});

            box->material->setAmbient(box->material, (vec3){1.0f, 0.5f, 0.31f});
            box->material->setDiffuse(box->material, 0);
            box->material->setSpecular(box->material, 2);
            box->material->setShininess(box->material, 32.0f);

            box->setScale(box, (vec3){10.0f, 0.01f, 10.0f});
            box->addTexture(box, texture);
            boxes->insertLast(boxes, box, true);
        }
    }

    engine->ground = boxes;

    // Tree
    // Default Material for tree
    Material* defaultMaterial = newMaterial();
    defaultMaterial->setAmbient(defaultMaterial, (vec3){1.0f, 0.5f, 0.31f});
    defaultMaterial->setDiffuse(defaultMaterial, 0);
    defaultMaterial->setSpecular(defaultMaterial, 1);
    defaultMaterial->setShininess(defaultMaterial, 32.0f);

    engine->textures->peekAt(engine->textures, 1, (void**)&texture, NULL);

    root = newBox((vec3){0.0f, -1.5f, 0.0f});

    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    for (i = 0; i < 4; i++)
    {
        box = newBox((vec3){0.0f, (float)(i + 1) - 1.5f, 0.0f});

        memcpy(box->material, defaultMaterial, sizeof(Material));
        box->addTexture(box, texture);
        root->attach(root, box);
    }

    engine->textures->peekAt(engine->textures, 2, (void**)&texture, NULL);
    box = newBox((vec3){0.0f, 3.0f, 0.0f});

    box->setScale(box, (vec3){3.0f, 2.0f, 3.0f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);
    engine->tree = root;

    // Wolf
    engine->textures->peekAt(engine->textures, 3, (void**)&texture, NULL);

    // Body
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){0.5f, 0.5f, 1.0});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Head
    box = newBox((vec3){0.0f, 0.0f, 0.6f});
    box->setScale(box, (vec3){0.35f, 0.35f, 0.35f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    // Legs
    box = newBox((vec3){-0.2f, -0.45f, -0.4f});
    box->setScale(box, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    box = newBox((vec3){-0.2f, -0.45f, 0.4f});
    box->setScale(box, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    box = newBox((vec3){0.2f, -0.45f, -0.4f});
    box->setScale(box, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    box = newBox((vec3){0.2f, -0.45f, 0.4f});
    box->setScale(box, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    // Tail
    box = newBox((vec3){0.0f, 0.2f, -0.7f});
    box->setScale(box, (vec3){0.1f, 0.1f, 0.4f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    // Head texture
    engine->textures->peekAt(engine->textures, 4, (void**)&texture, NULL);

    box = newBox((vec3){0.0f, 0.0f, 0.601f});
    box->setScale(box, (vec3){0.3499f, 0.3499f, 0.3499f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->wolf = root;

    // Sheep
    // Body
    engine->textures->peekAt(engine->textures, 5, (void**)&texture, NULL);

    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){1.25f, 1.25f, 2.0f});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Head
    box = newBox((vec3){0.0f, 0.4f, 1.25f});
    box->setScale(box, (vec3){0.7f, 0.7f, 0.7f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    // Legs
    engine->textures->peekAt(engine->textures, 6, (void**)&texture, NULL);
    box = newBox((vec3){-0.35f, -1.0f, -0.6f});
    box->setScale(box, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 5, (void**)&texture, NULL);
    box = newBox((vec3){-0.35f, -0.75f, -0.6f});
    box->setScale(box, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 6, (void**)&texture, NULL);
    box = newBox((vec3){0.35f, -1.0f, -0.6f});
    box->setScale(box, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 5, (void**)&texture, NULL);
    box = newBox((vec3){0.35f, -0.75f, -0.6f});
    box->setScale(box, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 6, (void**)&texture, NULL);
    box = newBox((vec3){-0.35f, -1.0f, 0.6f});
    box->setScale(box, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 5, (void**)&texture, NULL);
    box = newBox((vec3){-0.35f, -0.75f, 0.6f});
    box->setScale(box, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 6, (void**)&texture, NULL);
    box = newBox((vec3){0.35f, -1.0f, 0.6f});
    box->setScale(box, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->textures->peekAt(engine->textures, 5, (void**)&texture, NULL);
    box = newBox((vec3){0.35f, -0.75f, 0.6f});
    box->setScale(box, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    // Head texture
    engine->textures->peekAt(engine->textures, 7, (void**)&texture, NULL);

    box = newBox((vec3){0.0f, 0.4f, 1.251f});
    box->setScale(box, (vec3){0.699f, 0.699f, 0.699f});
    memcpy(box->material, defaultMaterial, sizeof(Material));
    box->addTexture(box, texture);

    root->attach(root, box);

    engine->sheep = root;
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

    ListNode* node;
    Box* box;
    Camera* cam;

    mat4 projection;
    mat4 view;

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
        glm_ortho(-((float)engine->width / 200.0f),
                   (float)engine->width / 200.0f,
                  -((float)engine->height / 200.0f),
                   (float)engine->height / 200.0f,
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

    FOR_EACH(engine->ground, node)
    {
        box = (Box*)node->value;
        box->setShader(box, normalShader);
        box->draw(box);
    }

    engine->tree->setShader(engine->tree, normalShader);
    for (int i = -50; i < 50; i += 10)
    {
        engine->tree->setPosition(engine->tree, (vec3){(float)i, -1.5f, 0.0f});
        engine->tree->draw(engine->tree);

        engine->tree->setPosition(engine->tree, (vec3){0.0f, -1.5f, (float)i});
        engine->tree->draw(engine->tree);
    }
    engine->tree->resetPosition(engine->tree);

    engine->wolf->setShader(engine->wolf, normalShader);
    engine->wolf->draw(engine->wolf);

    engine->sheep->setShader(engine->sheep, normalShader);
    engine->sheep->draw(engine->sheep);

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

    FOR_EACH(_engine->ground, iter)
    {
        box = (Box*)iter->value;
        box->destroy(box);
    }

    _engine->textures->deleteList(&(_engine->textures));
    _engine->shaders->deleteList(&(_engine->shaders));
    _engine->tree->destroy(_engine->tree);

    _engine->cam->destroy(_engine->cam);

    free(_engine);
    _engine = NULL;

    glfwTerminate();
}
