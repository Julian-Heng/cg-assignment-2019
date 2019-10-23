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
#include "models.h"
#include "shader.h"
#include "texture.h"

#include "game.h"


int main(void)
{
    bool ret = false;
    Backend* engine = init();
    loop(engine);
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

    // Mark safe zone for winning condition
    glm_vec3_copy((vec3){-20.0f, 0.0f, -20.0f}, engine->safeZone);

    // Init camera
    if (! (engine->cam = newCamera(engine->safeZone)))
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
    resetGameSettings(engine);

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
    char* filenames[] = {"shader"};

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
        "black",
        "grass",
        "grey",
        "red",
        "safe_zone",
        "sheep_face",
        "sheep_skin",
        "sign_1",
        "sign_2",
        "table",
        "tree_1",
        "tree_2",
        "white",
        "wolf_face"
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
    Material* shinyMaterial;
    engine->models = newHashTable();

    // Default Material
    defaultMaterial = newMaterial();
    defaultMaterial->setAmbient(defaultMaterial, (vec3){1.0f, 0.5f, 0.31f});
    defaultMaterial->setDiffuse(defaultMaterial, 0);
    defaultMaterial->setSpecular(defaultMaterial, 1);
    defaultMaterial->setShininess(defaultMaterial, 32.0f);

    // Shiny Material
    shinyMaterial = newMaterial();
    shinyMaterial->setAmbient(shinyMaterial, (vec3){0.19225f, 0.19225f, 0.19225f});
    shinyMaterial->setDiffuse(shinyMaterial, 0);
    shinyMaterial->setSpecular(shinyMaterial, 0);
    shinyMaterial->setShininess(shinyMaterial, 128.0f);

    // Make models
    initGround(engine, defaultMaterial);
    initTree(engine, defaultMaterial);
    initWolf(engine, defaultMaterial);
    initSheep(engine, defaultMaterial);
    initTable(engine, defaultMaterial, shinyMaterial);
    initTorch(engine, defaultMaterial, shinyMaterial);
    initSign(engine, defaultMaterial);
    initTrap(engine, shinyMaterial);
    initSafeZone(engine, shinyMaterial);

    SAFE_FREE(defaultMaterial);
    SAFE_FREE(shinyMaterial);
}


void resetGameSettings(Backend* engine)
{
    engine->options[GAME_USE_PERSPECTIVE] = true;
    engine->options[GAME_LIGHTS_ON] = false;
    engine->options[GAME_HAS_TORCH] = false;
    engine->options[GAME_PICKUP_WOLF] = false;
    engine->options[GAME_PLAYER_DIE] = false;
    engine->options[GAME_WIN] = false;

    engine->lightLevel = 1.0f;
}


void loop(Backend* engine)
{
    float lastTime = glfwGetTime();
    float currentTime;

    if (! engine)
    {
        return;
    }

    while (! glfwWindowShouldClose(engine->window))
    {
        logInfo(stderr, engine);
        instantKeyInputCallback(engine->window);

        currentTime = glfwGetTime();
        engine->timeDelta = currentTime - lastTime;
        lastTime = currentTime;

        // Set sky color depending on light setting
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
    vec3 sheepDirection = {0.0f, 0.0f, 1.0f};
    vec3 temp;
    float angle = 0.0f;

    Shader* shader;

    Box* model;
    Camera* cam;

    mat4 projection;
    mat4 view;

    cam = engine->cam;
    glm_mat4_identity(projection);
    glm_mat4_identity(view);
    shader = (Shader*)engine->shaders->search(engine->shaders, "shader");

    glfwGetWindowSize(engine->window, &(engine->width), &(engine->height));

    // Move camera down if player is dead
    if (engine->options[GAME_PLAYER_DIE])
    {
        engine->cam->setPosition(engine->cam,
                                 (vec3){engine->cam->position[X_COORD],
                                        -1.0f,
                                        engine->cam->position[Z_COORD]});
    }

    cam->getViewMatrix(cam, view);
    setupProjection(engine, cam, projection);
    setupShader(engine, shader, cam, projection, view);

    // Draw ground
    model = (Box*)engine->models->search(engine->models, "ground");
    model->setShader(model, shader);
    model->draw(model, NULL);

    // Draw trees
    model = (Box*)engine->models->search(engine->models, "tree");
    model->setShader(model, shader);
    for (int i = -50; i < 50; i += 10)
    {
        model->setPosition(model, (vec3){(float)i, 0.0f, 0.0f});
        model->draw(model, NULL);

        model->setPosition(model, (vec3){0.0f, 0.0f, (float)i});
        model->draw(model, NULL);
    }

    // Draw wolf
    if (! engine->options[GAME_PLAYER_DIE] &&
        ! engine->options[GAME_WIN])
    {
        model = (Box*)engine->models->search(engine->models, "wolf");
        model->setShader(model, shader);
        model->draw(model, (void*)engine);
    }

    // Draw sheep
    if (engine->options[GAME_PICKUP_WOLF] &&
        ! engine->options[GAME_PLAYER_DIE] &&
        ! engine->options[GAME_WIN])
    {
        model = (Box*)engine->models->search(engine->models, "sheep");
        model->setShader(model, shader);

        // Change angle and direction of vector depending on the player's
        // x coordinate and the sheep's x coordinate
        if (engine->cam->position[X_COORD] < model->position[X_COORD])
        {
            glm_vec3_sub(model->position, engine->cam->position, temp);
            angle = 180.0f;
        }
        else
        {
            glm_vec3_sub(engine->cam->position, model->position, temp);
        }

        // Rotate sheep to the camera
        angle += (180.0f * glm_vec3_angle(sheepDirection, temp)) / GLM_PI;
        model->setRotation(model, (vec3){0.0f, angle, 0.0f});

        // Slowly mode the sheep towards the camera
        glm_vec3_sub(engine->cam->position, model->position, temp);
        temp[Y_COORD] = 0.0f;
        glm_vec3_normalize(temp);
        glm_vec3_scale(temp, 0.09f, temp);
        model->move(model, temp);

        model->draw(model, (void*)engine);

        angle = 0.0f;

        // Check sheep's distance to player
        engine->options[GAME_PLAYER_DIE] = checkHitbox(engine, model->position, 2.0f);
    }

    // Draw traps
    if (engine->options[GAME_PICKUP_WOLF])
    {
        model = (Box*)engine->models->search(engine->models, "trap");
        model->setShader(model, shader);

        for (int i = -46; i < 46; i += 4)
        {
            model->setPosition(model, (vec3){(float)i, -2.0f, 0.0f});
            model->draw(model, NULL);

            model->setPosition(model, (vec3){0.0f, -2.0f, (float)i});
            model->draw(model, NULL);

            // Check if player touched a trap
            engine->options[GAME_PLAYER_DIE] = checkHitbox(engine, (vec3){(float)i, 0.0f, 0.0f}, 0.5f);
            engine->options[GAME_PLAYER_DIE] = checkHitbox(engine, (vec3){0.0f, 0.0f, (float)i}, 0.5f);
        }

        model->resetPosition(model);
    }

    // Draw table
    model = (Box*)engine->models->search(engine->models, "table");
    model->setShader(model, shader);
    model->draw(model, NULL);

    // Draw torch
    if (! engine->options[GAME_HAS_TORCH])
    {
        model = (Box*)engine->models->search(engine->models, "torch");

        // "Animate" torch
        model->move(model, (vec3){0.0f, sin(1.5f * glfwGetTime()) / 180.0f, 0.0f});
        model->setRotation(model, (vec3){0.0f, glfwGetTime() * 20.0f, 0.0f});

        model->setShader(model, shader);
        model->draw(model, NULL);
    }

    // Draw sign
    model = (Box*)engine->models->search(engine->models, "sign");
    model->setShader(model, shader);
    model->draw(model, NULL);

    // Draw safe zone
    model = (Box*)engine->models->search(engine->models, "safe_zone");
    model->setShader(model, shader);
    model->draw(model, NULL);

    cam->poll(cam);

    // Check win condition
    engine->options[GAME_WIN] = engine->options[GAME_PICKUP_WOLF] &&
                                checkHitbox(engine, engine->safeZone, 0.5f);
}


void drawWolfTail(Box* this, mat4 model, void* pointer)
{
    static bool direction = true;
    Backend* engine = (Backend*)pointer;

    glm_mat4_identity(model);

    glm_translate(model, this->position);

    // Rotate tail if picked up
    if (engine->options[GAME_PICKUP_WOLF])
    {
        // Change direction periodically
        direction = sin(glfwGetTime() * 8) > 0.0f;
        this->setRotationDelta(
            this, (vec3){0.0f, (direction ? -1 : 1) * 0.5f, 0.0f}
        );
    }

    glm_rotate_x(model, glm_rad(this->rotation[X_COORD]), model);
    glm_rotate_y(model, glm_rad(this->rotation[Y_COORD]), model);
    glm_rotate_z(model, glm_rad(this->rotation[Z_COORD]), model);

    glm_translate(model, this->modelPosition);

    glm_scale(model, this->scale);
}


void drawSheepLeg(Box* this, mat4 model, void* pointer)
{
    static int alternate = 0;
    float direction;

    glm_mat4_identity(model);

    glm_translate(model, this->position);

    glm_rotate_x(model, glm_rad(this->rotation[X_COORD]), model);
    glm_rotate_y(model, glm_rad(this->rotation[Y_COORD]), model);
    glm_rotate_z(model, glm_rad(this->rotation[Z_COORD]), model);

    // Alternate diagonal legs
    switch (alternate)
    {
        case 0: case 1: case 4: case 5: direction = -1.0f; break;
        case 2: case 3: case 6: case 7: direction = 1.0f; break;
    }

    // Change direction periodically
    direction *= sin(glfwGetTime() * 4) - 12.5f > 0.0f ? -1.0f : 1.0f;
    glm_rotate_x(model, direction * sin(glfwGetTime() * 4) / 5.0f, model);
    glm_translate(model, this->modelPosition);
    glm_scale(model, this->scale);

    // Alternate across legs diagonally
    alternate = (alternate + 1) % 8;
}


bool checkHitbox(Backend* engine, vec3 pos, float distance)
{
    if (engine->options[GAME_PLAYER_DIE])
        return true;

    return glm_vec3_distance(engine->cam->position, pos) < distance;
}


void setupProjection(Backend* engine, Camera* cam, mat4 projection)
{
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
}


void setupShader(Backend* engine, Shader* shader, Camera* cam,
                 mat4 projection, mat4 view)
{
    float light = engine->lightLevel;

    shader->use(shader);
    shader->setMat4(shader, "projection", projection);
    shader->setMat4(shader, "view", view);
    shader->setVec3(shader, "viewPos", cam->position);
    shader->setBool(shader, "lightsOn", engine->options[GAME_LIGHTS_ON]);

    if (engine->options[GAME_LIGHTS_ON])
    {
        shader->setVec3(shader, "light.ambient", (vec3){1.0f, 1.0f, 1.0f});
        shader->setVec3(shader, "light.diffuse", (vec3){1.0f, 1.0f, 1.0f});
        shader->setVec3(shader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});
    }
    else if (engine->options[GAME_HAS_TORCH])
    {
        shader->setVec3(shader, "light.ambient", (vec3){0.2f, 0.2f, 0.2f});
        shader->setVec3(shader, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f});
        shader->setVec3(shader, "light.specular", (vec3){1.0f, 1.0f, 1.0f});
    }
    else
    {
        shader->setVec3(shader, "light.ambient", (vec3){0.1f, 0.1f, 0.1f});
        shader->setVec3(shader, "light.diffuse", (vec3){0.0f, 0.0f, 0.0f});
        shader->setVec3(shader, "light.specular", (vec3){0.1f, 0.1f, 0.1f});
    }

    shader->setFloat(shader, "light.constant", 1.0f);
    shader->setFloat(shader, "light.linear", 0.09f);
    shader->setFloat(shader, "light.quadratic", 0.032f);

    shader->setVec3(shader, "light.position", cam->position);
    shader->setVec3(shader, "light.direction", cam->front);
    shader->setFloat(shader, "light.cutOff", cos(glm_rad(light * 17.5f)));
    shader->setFloat(shader, "light.outerCutOff", cos(glm_rad(light * 26.25f)));
}


void toggleWireframe()
{
    static int toggle = 0;
    toggle = (toggle + 1) % 3;
    glPolygonMode(GL_FRONT_AND_BACK, toggle == 1 ? GL_LINE :
                                     toggle == 2 ? GL_POINT :
                                     GL_FILL);
}


void normalInputCallback(GLFWwindow* win, int key, int scancode,
                         int action, int mods)
{
    vec3 temp;
    Box* model;
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);

    if (action != GLFW_PRESS || ! engine)
    {
        return;
    }

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:    glfwSetWindowShouldClose(win, true); break;
        case GLFW_KEY_TAB:  toggleWireframe(); break;
        case GLFW_KEY_P:    engine->options[GAME_USE_PERSPECTIVE] ^= 1 ; break;
        case GLFW_KEY_O:    engine->options[GAME_LIGHTS_ON] ^= 1; break;

        case GLFW_KEY_K:
            // Change light level if player has torch
            if (engine->options[GAME_HAS_TORCH])
            {
                engine->lightLevel = MAX(engine->lightLevel - 0.1f, 0.0f);
            }

            break;

        case GLFW_KEY_L:
            // Change light level if player has torch
            if (engine->options[GAME_HAS_TORCH])
            {
                engine->lightLevel = MIN(engine->lightLevel + 0.1f, 2.0f);
            }

            break;

        case GLFW_KEY_F:
            model = (Box*)engine->models->search(engine->models, "torch");

            // Set new position for the torch
            if (engine->options[GAME_HAS_TORCH])
            {
                glm_vec3_copy(engine->cam->front, temp);
                glm_vec3_normalize_to((vec3){temp[X_COORD], 0.0f, temp[Z_COORD]}, temp);
                glm_vec3_scale(temp, 2.0f, temp);
                glm_vec3_add(engine->cam->position, temp, temp);

                model->setPosition(model, temp);
                engine->options[GAME_HAS_TORCH] = false;
            }
            else if (checkHitbox(engine, model->position, 3.0f))
            {
                engine->options[GAME_HAS_TORCH] = true;
            }

            break;


        case GLFW_KEY_E:
            model = (Box*)engine->models->search(engine->models, "wolf");

            // Drop wolf
            if (engine->options[GAME_PICKUP_WOLF])
            {
                engine->cam->detach(engine->cam);

                // Set new position for the wolf
                glm_vec3_copy(engine->cam->front, temp);
                glm_vec3_normalize_to((vec3){temp[X_COORD], 0.0f, temp[Z_COORD]}, temp);
                glm_vec3_scale(temp, 2.0f, temp);
                glm_vec3_add(engine->cam->position, temp, temp);
                temp[Y_COORD] = -1.35f;

                model->setPosition(model, temp);
                engine->options[GAME_PICKUP_WOLF] = false;
            }
            else if (checkHitbox(engine, model->position, 3.0f))
            {
                // Pickup wolf
                engine->cam->attach(engine->cam, model);
                engine->options[GAME_PICKUP_WOLF] = true;
            }

            break;
    }
}


void instantKeyInputCallback(GLFWwindow* win)
{
    Backend* engine = (Backend*)glfwGetWindowUserPointer(win);
    Camera* cam = engine ? engine->cam : NULL;

    Box* box;
    HashEntry* iter;

    if (! cam)
    {
        return;
    }

    float timeDelta = engine->timeDelta;

    bool keys[] = {
        KEY_PRESSED(win, GLFW_KEY_W) && CHECK_GAME_STATE(engine),
        KEY_PRESSED(win, GLFW_KEY_A) && CHECK_GAME_STATE(engine),
        KEY_PRESSED(win, GLFW_KEY_S) && CHECK_GAME_STATE(engine),
        KEY_PRESSED(win, GLFW_KEY_D) && CHECK_GAME_STATE(engine),
        KEY_PRESSED(win, GLFW_KEY_SPACE) && CHECK_GAME_STATE(engine),

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

    // Reset game state
    if (keys[GAME_RESET])
    {
        resetGameSettings(engine);

        cam->setJump(cam, false);
        cam->resetPosition(cam);
        cam->resetFront(cam);
        cam->detach(cam);

        HASHTABLE_FOR_EACH(engine->models, iter)
        {
            box = (Box*)iter->value;
            box->resetPosition(box);
            box->resetRotation(box);
        }
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
    glViewport(0, 0, width, height);
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
