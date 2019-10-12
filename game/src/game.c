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
#include "hashtable.h"
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

    engine->options[GAME_USE_PERSPECTIVE] = true;
    engine->options[GAME_LIGHTS_ON] = false;

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
    HashTable* shaders = newHashTable();
    char* filenames[] = {"shader", "lamp"};

    for (int i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++)
    {
        char vertexFilename[BUFSIZ];
        char fragmentFilename[BUFSIZ];

        snprintf(vertexFilename, BUFSIZ, "shaders/%s.vs", filenames[i]);
        snprintf(fragmentFilename, BUFSIZ, "shaders/%s.fs", filenames[i]);

        shaders->insert(
            shaders,
            filenames[i],
            newShader(vertexFilename, fragmentFilename),
            true
        );
    }

    engine->shaders = shaders;
}


void initTextures(Backend* engine)
{
    HashTable* textures = newHashTable();
    char* filenames[] = {
        "grass", "tree_1", "tree_2", "grey",
        "wolf_face", "black", "sheep_skin", "sheep_face"
    };

    for (int i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++)
    {
        char filename[BUFSIZ];
        snprintf(filename, BUFSIZ, "resources/%s.png", filenames[i]);
        textures->insert(
            textures,
            filenames[i],
            newTexture(filename, GL_RGBA, false),
            true
        );
    }

    engine->textures = textures;
}


void initShapes(Backend* engine)
{
    Material* defaultMaterial;
    engine->models = newHashTable();

    // Default Material
    defaultMaterial = newMaterial();
    defaultMaterial->setAmbient(defaultMaterial, (vec3){1.0f, 0.5f, 0.31f});
    defaultMaterial->setDiffuse(defaultMaterial, 0);
    defaultMaterial->setSpecular(defaultMaterial, 1);
    defaultMaterial->setShininess(defaultMaterial, 32.0f);

    initGround(engine, defaultMaterial);
    initTree(engine, defaultMaterial);
    initWolf(engine, defaultMaterial);
    initSheep(engine, defaultMaterial);
}


void initGround(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "grass");

    bool first = true;
    for (int i = -50; i < 50; i += 10)
    {
        for (int j = -50; j < 50; j += 10)
        {
            model = newBox((vec3){(float)i, -2.0f, (float)j});
            memcpy(model->material, defaultMaterial, sizeof(Material));
            model->setScale(model, (vec3){10.0f, 0.01f, 10.0f});
            model->addTexture(model, texture);

            if (first)
            {
                root = model;
                first = false;
            }
            else
            {
                root->attach(root, model);
            }
        }
    }

    engine->models->insert(engine->models, "ground", root, true);
}


void initTree(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "tree_1");

    root = newBox((vec3){0.0f, -1.5f, 0.0f});

    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    for (int i = 0; i < 4; i++)
    {
        model = newBox((vec3){0.0f, (float)(i + 1) - 1.5f, 0.0f});

        memcpy(model->material, defaultMaterial, sizeof(Material));
        model->addTexture(model, texture);
        root->attach(root, model);
    }

    texture = engine->textures->search(engine->textures, "tree_2");
    model = newBox((vec3){0.0f, 3.0f, 0.0f});

    model->setScale(model, (vec3){3.0f, 2.0f, 3.0f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);
    engine->models->insert(engine->models, "tree", root, true);
}


void initWolf(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "grey");

    // Body
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){0.5f, 0.5f, 1.0});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Head
    model = newBox((vec3){0.0f, 0.0f, 0.6f});
    model->setScale(model, (vec3){0.35f, 0.35f, 0.35f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    // Legs
    model = newBox((vec3){-0.2f, -0.45f, -0.4f});
    model->setScale(model, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    model = newBox((vec3){-0.2f, -0.45f, 0.4f});
    model->setScale(model, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    model = newBox((vec3){0.2f, -0.45f, -0.4f});
    model->setScale(model, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    model = newBox((vec3){0.2f, -0.45f, 0.4f});
    model->setScale(model, (vec3){0.1f, 0.4f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    // Tail
    model = newBox((vec3){0.0f, 0.2f, -0.7f});
    model->setScale(model, (vec3){0.1f, 0.1f, 0.4f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    // Head texture
    texture = engine->textures->search(engine->textures, "wolf_face");

    model = newBox((vec3){0.0f, 0.0f, 0.601f});
    model->setScale(model, (vec3){0.3499f, 0.3499f, 0.3499f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    engine->models->insert(engine->models, "wolf", root, true);
}


void initSheep(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    Box* model2;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "black");

    // Body
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){1.25f, 1.25f, 2.0f});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Head
    model = newBox((vec3){0.0f, 0.4f, 1.25f});
    model->setScale(model, (vec3){0.7f, 0.7f, 0.7f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    // Legs
    texture = engine->textures->search(engine->textures, "black");
    model = newBox((vec3){-0.35f, -0.75f, -0.6f});
    model->setScale(model, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    texture = engine->textures->search(engine->textures, "sheep_skin");
    model2 = newBox((vec3){-0.35f, -1.0f, -0.6f});
    model2->setScale(model2, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(model2->material, defaultMaterial, sizeof(Material));
    model2->addTexture(model2, texture);

    model->attach(model, model2);
    root->attach(root, model);

    texture = engine->textures->search(engine->textures, "black");
    model = newBox((vec3){0.35f, -0.75f, -0.6f});
    model->setScale(model, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    texture = engine->textures->search(engine->textures, "sheep_skin");
    model2 = newBox((vec3){0.35f, -1.0f, -0.6f});
    model2->setScale(model2, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(model2->material, defaultMaterial, sizeof(Material));
    model2->addTexture(model2, texture);

    model->attach(model, model2);
    root->attach(root, model);

    texture = engine->textures->search(engine->textures, "black");
    model = newBox((vec3){-0.35f, -0.75f, 0.6f});
    model->setScale(model, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    texture = engine->textures->search(engine->textures, "sheep_skin");
    model2 = newBox((vec3){-0.35f, -1.0f, 0.6f});
    model2->setScale(model2, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(model2->material, defaultMaterial, sizeof(Material));
    model2->addTexture(model2, texture);

    model->attach(model, model2);
    root->attach(root, model);

    texture = engine->textures->search(engine->textures, "black");
    model = newBox((vec3){0.35f, -0.75f, 0.6f});
    model->setScale(model, (vec3){0.3f, 0.4f, 0.3f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    texture = engine->textures->search(engine->textures, "sheep_skin");
    model2 = newBox((vec3){0.35f, -1.0f, 0.6f});
    model2->setScale(model2, (vec3){0.25f, 0.8f, 0.25f});
    memcpy(model2->material, defaultMaterial, sizeof(Material));
    model2->addTexture(model2, texture);

    model->attach(model, model2);
    root->attach(root, model);

    // Head texture
    texture = engine->textures->search(engine->textures, "sheep_face");

    model = newBox((vec3){0.0f, 0.4f, 1.251f});
    model->setScale(model, (vec3){0.699f, 0.699f, 0.699f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    engine->models->insert(engine->models, "sheep", root, true);
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

        if (! engine->options[GAME_LIGHTS_ON])
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

    Box* model;
    Camera* cam;

    mat4 projection;
    mat4 view;

    cam = engine->cam;

    normalShader = engine->shaders->search(engine->shaders, "shader");

    glfwGetWindowSize(engine->window, &(engine->width), &(engine->height));

    glm_mat4_identity(projection);
    glm_mat4_identity(view);

    normalShader->use(normalShader);

    cam->getViewMatrix(cam, view);

    if (engine->options[GAME_USE_PERSPECTIVE])
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

    normalShader->setBool(normalShader, "lightsOn", engine->options[GAME_LIGHTS_ON]);

    if (! engine->options[GAME_LIGHTS_ON])
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

    model = engine->models->search(engine->models, "ground");
    model->setShader(model, normalShader);
    model->draw(model);

    model = engine->models->search(engine->models, "tree");
    model->setShader(model, normalShader);
    for (int i = -50; i < 50; i += 10)
    {
        model->setPosition(model, (vec3){(float)i, -1.5f, 0.0f});
        model->draw(model);

        model->setPosition(model, (vec3){0.0f, -1.5f, (float)i});
        model->draw(model);
    }
    model->resetPosition(model);

    model = engine->models->search(engine->models, "wolf");
    model->setShader(model, normalShader);
    model->draw(model);

    model = engine->models->search(engine->models, "sheep");
    model->setShader(model, normalShader);
    model->draw(model);

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
            engine->options[GAME_USE_PERSPECTIVE] ^= 1 ;
            break;

        case GLFW_KEY_O:
            engine->options[GAME_LIGHTS_ON] ^= 1;
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
    HashEntry* iter;

    glDeleteVertexArrays(1, &(_engine->VAO));
    glDeleteBuffers(1, &(_engine->VBO));

    if (! _engine)
    {
        return;
    }

    HASHTABLE_FOR_EACH(_engine->models, iter)
    {
        box = (Box*)iter->value;
        box->destroy(box);
        _engine->models->delete(_engine->models, iter->key);
    }

    _engine->textures->deleteHashTable(&(_engine->textures));
    _engine->shaders->deleteHashTable(&(_engine->shaders));

    _engine->cam->destroy(_engine->cam);

    free(_engine);
    _engine = NULL;

    glfwTerminate();
}
