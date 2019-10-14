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

    if (! (engine->cam = newCamera((vec3){-20.0f, 0.0f, -20.0f})))
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
    engine->options[GAME_HAS_TORCH] = false;
    engine->options[GAME_PICKUP_WOLF] = false;

    engine->lightLevel = 1.0f;

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
        "black",
        "grass",
        "grey",
        "red",
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

    initGround(engine, defaultMaterial);
    initTree(engine, defaultMaterial);
    initWolf(engine, defaultMaterial);
    initSheep(engine, defaultMaterial);
    initTable(engine, defaultMaterial, shinyMaterial);
    initTorch(engine, defaultMaterial, shinyMaterial);
    initSign(engine, defaultMaterial);

    SAFE_FREE(defaultMaterial);
    SAFE_FREE(shinyMaterial);
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

    // Trunk
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

    // Leaves
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
    for (int i = -1; i < 2; i += 2)
    {
        for (int j = -1; j < 2; j += 2)
        {
            model = newBox((vec3){-0.2f * (float)i, -0.45f, -0.4f * (float)j});
            model->setScale(model, (vec3){0.1f, 0.4f, 0.1f});
            memcpy(model->material, defaultMaterial, sizeof(Material));
            model->addTexture(model, texture);
            root->attach(root, model);
        }
    }

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

    // Move to position
    root->setPosition(root, (vec3){25.0f, -1.35f, 25.0f});

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
    for (int i = -1; i < 2; i += 2)
    {
        for (int j = -1; j < 2; j += 2)
        {
            texture = engine->textures->search(engine->textures, "black");
            model = newBox((vec3){-0.35f * (float)i, -0.75f, -0.6f * (float)j});
            model->setScale(model, (vec3){0.3f, 0.4f, 0.3f});
            memcpy(model->material, defaultMaterial, sizeof(Material));
            model->addTexture(model, texture);

            texture = engine->textures->search(engine->textures, "sheep_skin");
            model2 = newBox((vec3){-0.35f * (float)i, -1.0f, -0.6f * (float)j});
            model2->setScale(model2, (vec3){0.25f, 0.8f, 0.25f});
            memcpy(model2->material, defaultMaterial, sizeof(Material));
            model2->addTexture(model2, texture);

            model->attach(model, model2);
            root->attach(root, model);
        }
    }

    // Head texture
    texture = engine->textures->search(engine->textures, "sheep_face");

    model = newBox((vec3){0.0f, 0.4f, 1.251f});
    model->setScale(model, (vec3){0.699f, 0.699f, 0.699f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);

    engine->models->insert(engine->models, "sheep", root, true);
}


void initTable(Backend* engine, Material* defaultMaterial, Material* shinyMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "table");

    // Table top
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){2.0f, 0.1f, 2.0f});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Table legs
    for (int i = -1; i < 2; i += 2)
    {
        for (int j = -1; j < 2; j += 2)
        {
            texture = (Texture*)engine->textures->search(engine->textures, "black");
            model = newBox((vec3){-0.8f * (float)i, -0.675f, -0.8f * (float)j});
            model->setScale(model, (vec3){0.1f, 1.25f, 0.1f});
            memcpy(model->material, shinyMaterial, sizeof(Material));
            model->addTexture(model, texture);
            root->attach(root, model);
        }
    }

    // Move to position
    root->setPosition(root, (vec3){-25.0f, -0.7f, 25.0f});

    engine->models->insert(engine->models, "table", root, true);
}


void initTorch(Backend* engine, Material* defaultMaterial, Material* shinyMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "black");

    // Torch "body"
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){0.1f, 0.5f, 0.1f});
    memcpy(root->material, shinyMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Torch Light
    texture = (Texture*)engine->textures->search(engine->textures, "white");
    model = newBox((vec3){0.0f, 0.2f, 0.0f});
    model->setScale(model, (vec3){0.11f, 0.11f, 0.11f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);
    root->attach(root, model);

    // Torch button
    texture = (Texture*)engine->textures->search(engine->textures, "red");
    model = newBox((vec3){0.0f, 0.0f, 0.05f});
    model->setScale(model, (vec3){0.025f, 0.05f, 0.025f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);
    root->attach(root, model);

    // Move to position
    root->setPosition(root, (vec3){-25.0f, -0.2f, 25.0f});

    engine->models->insert(engine->models, "torch", root, true);
}


void initSign(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    Texture* texture = (Texture*)engine->textures->search(engine->textures, "sign_1");

    // Sign post
    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){0.1f, 1.5f, 0.1f});
    memcpy(root->material, defaultMaterial, sizeof(Material));
    root->addTexture(root, texture);

    // Sign back
    model = newBox((vec3){0.0f, 0.5f, 0.05f});
    model->setScale(model, (vec3){1.0f, 1.0f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);
    root->attach(root, model);

    // Sign message
    texture = (Texture*)engine->textures->search(engine->textures, "sign_2");
    model = newBox((vec3){0.0f, 0.5f, 0.051f});
    model->setScale(model, (vec3){0.999f, 0.999f, 0.1f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);
    root->attach(root, model);

    // Move to position
    root->setPosition(root, (vec3){-20.0f, -1.25f, -25.0f});

    engine->models->insert(engine->models, "sign", root, true);
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
    Shader* shader;

    Box* model;
    Camera* cam;

    mat4 projection;
    mat4 view;

    cam = engine->cam;
    glm_mat4_identity(projection);
    glm_mat4_identity(view);
    shader = engine->shaders->search(engine->shaders, "shader");

    glfwGetWindowSize(engine->window, &(engine->width), &(engine->height));

    cam->getViewMatrix(cam, view);
    setupProjection(engine, cam, projection);
    setupShader(engine, shader, cam, projection, view);

    // Draw ground
    model = engine->models->search(engine->models, "ground");
    model->setShader(model, shader);
    model->draw(model);

    // Draw trees
    model = engine->models->search(engine->models, "tree");
    model->setShader(model, shader);
    for (int i = -50; i < 50; i += 10)
    {
        model->setPosition(model, (vec3){(float)i, 0.0f, 0.0f});
        model->draw(model);

        model->setPosition(model, (vec3){0.0f, 0.0f, (float)i});
        model->draw(model);
    }
    model->resetPosition(model);

    // Draw wolf
    model = engine->models->search(engine->models, "wolf");
    model->setShader(model, shader);
    model->draw(model);

    // Draw sheep
    if (engine->options[GAME_PICKUP_WOLF])
    {
        model = engine->models->search(engine->models, "sheep");
        model->setShader(model, shader);
        model->draw(model);
    }

    // Draw table
    model = engine->models->search(engine->models, "table");
    model->setShader(model, shader);
    model->draw(model);

    // Draw torch
    if (! engine->options[GAME_HAS_TORCH])
    {
        model = engine->models->search(engine->models, "torch");

        // "Animate" torch
        model->move(model, (vec3){0.0f, sin(1.5f * glfwGetTime()) / 180.0f, 0.0f});
        model->setRotation(model, (vec3){0.0f, glfwGetTime() * 20.0f, 0.0f});

        model->setShader(model, shader);
        model->draw(model);
    }

    // Draw sign
    model = engine->models->search(engine->models, "sign");
    model->setShader(model, shader);
    model->draw(model);

    cam->poll(cam);
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
    float lightLevel = engine->lightLevel;

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
    shader->setFloat(shader, "light.cutOff", cos(glm_rad(lightLevel * 17.5f)));
    shader->setFloat(shader, "light.outerCutOff", cos(glm_rad(lightLevel * 26.25f)));
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
            if (engine->options[GAME_HAS_TORCH])
            {
                engine->lightLevel = MAX(engine->lightLevel - 0.1f, 0.0f);
            }

            break;

        case GLFW_KEY_L:
            if (engine->options[GAME_HAS_TORCH])
            {
                engine->lightLevel = MIN(engine->lightLevel + 0.1f, 2.0f);
            }

            break;

        case GLFW_KEY_F:
            model = engine->models->search(engine->models, "torch");

            if (engine->options[GAME_HAS_TORCH])
            {
                // Set new position for the torch
                glm_vec3_copy(engine->cam->front, temp);
                glm_vec3_normalize_to((vec3){temp[0], 0.0f, temp[2]}, temp);
                glm_vec3_scale(temp, 2.0f, temp);
                glm_vec3_add(engine->cam->position, temp, temp);

                model->setPosition(model, temp);
                engine->options[GAME_HAS_TORCH] = false;
            }
            else if (glm_vec3_distance(engine->cam->position, model->position) < 3.0f)
            {
                engine->options[GAME_HAS_TORCH] = true;
            }

            break;


        case GLFW_KEY_E:
            model = engine->models->search(engine->models, "wolf");

            if (engine->options[GAME_PICKUP_WOLF])
            {
                engine->cam->detach(engine->cam);

                // Set new position for the wolf
                glm_vec3_copy(engine->cam->front, temp);
                glm_vec3_normalize_to((vec3){temp[0], 0.0f, temp[2]}, temp);
                glm_vec3_scale(temp, 2.0f, temp);
                glm_vec3_add(engine->cam->position, temp, temp);
                temp[1] = -1.35f;

                model->setPosition(model, temp);
                model->setRotateLast(model, false);
                engine->options[GAME_PICKUP_WOLF] = false;
            }
            else if (glm_vec3_distance(engine->cam->position, model->position) < 3.0f)
            {
                model->setRotateLast(model, true);
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

    if (keys[GAME_RESET])
    {
        engine->options[GAME_USE_PERSPECTIVE] = true;
        engine->options[GAME_LIGHTS_ON] = false;
        engine->options[GAME_HAS_TORCH] = false;
        engine->options[GAME_PICKUP_WOLF] = false;

        engine->lightLevel = 1.0f;

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
