#include <cglm/vec3.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "box.h"
#include "game.h"
#include "hashtable.h"
#include "macros.h"
#include "material.h"
#include "texture.h"

#include "models.h"


void initGround(Backend* engine, Material* defaultMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;
    Texture* texture = (Texture*)textures->search(textures, "grass");

    for (int i = -50; i < 50; i += 10)
    {
        for (int j = -50; j < 50; j += 10)
        {
            model = newBox((vec3){(float)i, -2.0f, (float)j});
            memcpy(model->material, defaultMaterial, sizeof(Material));
            model->setScale(model, (vec3){10.0f, 0.01f, 10.0f});
            model->addTexture(model, texture);

            if (root)
            {
                root->attach(root, model);
            }
            else
            {
                root = model;
            }
        }
    }

    engine->models->insert(engine->models, "ground", root, true);
}


void initTree(Backend* engine, Material* defaultMaterial)
{
    Box* root;
    Box* model;
    HashTable* textures = engine->textures;

    Texture* texture = (Texture*)textures->search(textures, "tree_1");

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
    texture = (Texture*)textures->search(textures, "tree_2");
    model = newBox((vec3){0.0f, 3.0f, 0.0f});
    model->setScale(model, (vec3){3.0f, 2.0f, 3.0f});
    memcpy(model->material, defaultMaterial, sizeof(Material));
    model->addTexture(model, texture);

    root->attach(root, model);
    engine->models->insert(engine->models, "tree", root, true);
}


void initWolf(Backend* engine, Material* defaultMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "grey");
    Texture* texture2 = (Texture*)textures->search(textures, "wolf_face");

    vec3 specifications[][2] = {
        {{0.0f, 0.0f, 0.0f}, {0.5f,  0.5f,  1.0f}},  // Body
        {{0.0f, 0.0f, 0.6f}, {0.35f, 0.35f, 0.35f}}, // Head

        {{-0.2f, -0.45f, -0.4f}, {0.1f, 0.4f, 0.1f}}, // Legs
        {{0.2f,  -0.45f, -0.4f}, {0.1f, 0.4f, 0.1f}}, // Legs
        {{-0.2f, -0.45f,  0.4f}, {0.1f, 0.4f, 0.1f}}, // Legs
        {{0.2f,  -0.45f,  0.4f}, {0.1f, 0.4f, 0.1f}}, // Legs

        {{0.0f, 0.2f, -0.7f},   {0.1f,    0.1f,    0.4f}},   // Tail
        {{0.0f, 0.0f,  0.601f}, {0.3499f, 0.3499f, 0.3499f}} // Head
    };

    Texture* textureMap[] = {
        texture1, // Body
        texture1, // Head

        texture1, // Legs
        texture1, // Legs
        texture1, // Legs
        texture1, // Legs

        texture1, // Tail
        texture2  // Head
    };

    Material* materialMap[] = {
        defaultMaterial, // Body
        defaultMaterial, // Head

        defaultMaterial, // Legs
        defaultMaterial, // Legs
        defaultMaterial, // Legs
        defaultMaterial, // Legs

        defaultMaterial, // Tail
        defaultMaterial  // Head
    };

    void (*drawingFuncs[])(Box*, mat4, void*) = {
        NULL,
        NULL,

        NULL,
        NULL,
        NULL,
        NULL,

        drawWolfTail,
        NULL
    };

    MAKE_MODEL(root, model, specifications, textureMap, materialMap, drawingFuncs);

    // Move to position
    //root->setPosition(root, (vec3){25.0f, -1.35f, 25.0f});
    root->setPosition(root, (vec3){-25.0f, -1.35f, -25.0f});
    root->recordInitialPosition(root);
    root->recordInitialRotation(root);

    engine->models->insert(engine->models, "wolf", root, true);
}


void initSheep(Backend* engine, Material* defaultMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "black");
    Texture* texture2 = (Texture*)textures->search(textures, "sheep_skin");
    Texture* texture3 = (Texture*)textures->search(textures, "sheep_face");

    vec3 specifications[][2] = {
        {{0.0f, 0.0f, 0.0f},  {1.25f, 1.25f, 2.0f}}, // Body
        {{0.0f, 0.4f, 1.25f}, {0.7f,  0.7f,  0.7f}}, // Head

        {{-0.35f, -0.75f, -0.6f}, {0.3f,  0.4f, 0.3f}},  // Legs
        {{-0.35f, -1.0f,  -0.6f}, {0.25f, 0.8f, 0.25f}}, // Legs
        {{0.35f,  -0.75f, -0.6f}, {0.3f,  0.4f, 0.3f}},  // Legs
        {{0.35f,  -1.0f,  -0.6f}, {0.25f, 0.8f, 0.25f}}, // Legs
        {{0.35f,  -0.75f,  0.6f}, {0.3f,  0.4f, 0.3f}},  // Legs
        {{0.35f,  -1.0f,   0.6f}, {0.25f, 0.8f, 0.25f}}, // Legs
        {{-0.35f, -0.75f,  0.6f}, {0.3f,  0.4f, 0.3f}},  // Legs
        {{-0.35f, -1.0f,   0.6f}, {0.25f, 0.8f, 0.25f}}, // Legs

        {{0.0f, 0.4f, 1.251f}, {0.699f, 0.699f, 0.699f}}, // Head
    };

    Texture* textureMap[] = {
        texture1, // Body
        texture1, // Head

        texture1, texture2, // Legs
        texture1, texture2, // Legs
        texture1, texture2, // Legs
        texture1, texture2, // Legs

        texture3  // Head
    };

    Material* materialMap[] = {
        defaultMaterial,
        defaultMaterial,

        defaultMaterial, defaultMaterial,
        defaultMaterial, defaultMaterial,
        defaultMaterial, defaultMaterial,
        defaultMaterial, defaultMaterial,

        defaultMaterial
    };

    void (*drawingFuncs[])(Box*, mat4, void*) = {
        NULL,
        NULL,

        drawSheepLeg, drawSheepLeg,
        drawSheepLeg, drawSheepLeg,
        drawSheepLeg, drawSheepLeg,
        drawSheepLeg, drawSheepLeg,

        NULL,
    };

    MAKE_MODEL(root, model, specifications, textureMap, materialMap, drawingFuncs);

    root->setPosition(root, (vec3){25.0f, -0.6f, -25.0f});

    engine->models->insert(engine->models, "sheep", root, true);
}


void initTable(Backend* engine, Material* defaultMaterial, Material* shinyMaterial)
{
    Box* root;
    Box* model;
    HashTable* textures = engine->textures;
    Texture* texture = (Texture*)textures->search(textures, "table");

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
            texture = (Texture*)textures->search(textures, "black");
            model = newBox((vec3){-0.8f * (float)i, -0.675f, -0.8f * (float)j});
            model->setScale(model, (vec3){0.1f, 1.25f, 0.1f});
            memcpy(model->material, shinyMaterial, sizeof(Material));
            model->addTexture(model, texture);
            root->attach(root, model);
        }
    }

    // Move to position
    root->setPosition(root, (vec3){-25.0f, -0.7f, 25.0f});
    root->recordInitialPosition(root);
    root->recordInitialRotation(root);

    engine->models->insert(engine->models, "table", root, true);
}


void initTorch(Backend* engine, Material* defaultMaterial, Material* shinyMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "black");
    Texture* texture2 = (Texture*)textures->search(textures, "white");
    Texture* texture3 = (Texture*)textures->search(textures, "red");

    vec3 specifications[][2] = {
        {{0.0f, 0.0f, 0.0f},  {0.1f,   0.5f,  0.1f}},
        {{0.0f, 0.2f, 0.0f},  {0.11f,  0.11f, 0.11f}},
        {{0.0f, 0.0f, 0.05f}, {0.025f, 0.05f, 0.025f}}
    };

    Texture* textureMap[] = {texture1, texture2, texture3};
    Material* materialMap[] = {shinyMaterial, defaultMaterial, defaultMaterial};
    void (*drawingFuncs[])(Box*, mat4, void*) = {NULL, NULL, NULL};
    MAKE_MODEL(root, model, specifications, textureMap, materialMap, drawingFuncs);

    // Move to position
    root->setPosition(root, (vec3){-25.0f, -0.2f, 25.0f});
    root->recordInitialPosition(root);
    root->recordInitialRotation(root);

    engine->models->insert(engine->models, "torch", root, true);
}


void initSign(Backend* engine, Material* defaultMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "sign_1");
    Texture* texture2 = (Texture*)textures->search(textures, "sign_2");

    vec3 specifications[][2] = {
        {{0.0f, 0.0f, 0.0f},   {0.1f,    1.5f,    0.1f}},
        {{0.0f, 0.5f, 0.05f},  {1.0f,    1.0f,    0.1f}},
        {{0.0f, 0.5f, 0.051f}, {0.9999f, 0.9999f, 0.1f}}
    };

    Texture* textureMap[] = {texture1, texture1, texture2};
    Material* materialMap[] = {defaultMaterial, defaultMaterial, defaultMaterial};
    void (*drawingFuncs[])(Box*, mat4, void*) = {NULL, NULL, NULL};
    MAKE_MODEL(root, model, specifications, textureMap, materialMap, drawingFuncs);

    // Move to position
    root->setPosition(root, (vec3){-20.0f, -1.25f, -25.0f});
    root->recordInitialPosition(root);
    root->recordInitialRotation(root);

    engine->models->insert(engine->models, "sign", root, true);
}


void initTrap(Backend* engine, Material* shinyMaterial)
{
    Box* root = NULL;
    Box* model = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "black");

    vec3 specifications[][2] = {
        {{0.0f, 0.0f, 0.0f}, {1.0f, 0.05f, 1.0f}},  // Base

        // Teeth
        {{-0.475f, 0.075f, -0.475f}, {0.05f, 0.1f, 0.05f}},

        {{-0.475f, 0.075f, -0.2375f}, {0.05f, 0.1f, 0.05f}},
        {{-0.475f, 0.075f,  0.0f},    {0.05f, 0.1f, 0.05f}},
        {{-0.475f, 0.075f,  0.2375f}, {0.05f, 0.1f, 0.05f}},
        {{-0.475f, 0.075f,  0.475f},  {0.05f, 0.1f, 0.05f}},

        {{-0.2375f, 0.075f, 0.475f}, {0.05f, 0.1f, 0.05f}},
        {{0.0f,     0.075f, 0.475f}, {0.05f, 0.1f, 0.05f}},
        {{0.2375,   0.075f, 0.475f}, {0.05f, 0.1f, 0.05f}},
        {{0.475f,   0.075f, 0.475f}, {0.05f, 0.1f, 0.05f}},

        {{0.475f, 0.075f,  0.2375f}, {0.05f, 0.1f, 0.05f}},
        {{0.475f, 0.075f,  0.0f},    {0.05f, 0.1f, 0.05f}},
        {{0.475f, 0.075f, -0.2375f}, {0.05f, 0.1f, 0.05f}},
        {{0.475f, 0.075f, -0.475f},  {0.05f, 0.1f, 0.05f}},

        {{0.2375f, 0.075f, -0.475f}, {0.05f, 0.1f, 0.05f}},
        {{0.0f,    0.075f, -0.475f}, {0.05f, 0.1f, 0.05f}},
        {{-0.2375, 0.075f, -0.475f}, {0.05f, 0.1f, 0.05f}},
        {{-0.475f, 0.075f, -0.475f}, {0.05f, 0.1f, 0.05f}}
    };

    Texture* textureMap[] = {
        texture1,   // Base
        texture1,   // Teeth

        texture1, texture1, texture1, texture1,
        texture1, texture1, texture1, texture1,
        texture1, texture1, texture1, texture1,
        texture1, texture1, texture1, texture1
    };

    Material* materialMap[] = {
        shinyMaterial,  // Base
        shinyMaterial,  // Teeth

        shinyMaterial, shinyMaterial, shinyMaterial, shinyMaterial,
        shinyMaterial, shinyMaterial, shinyMaterial, shinyMaterial,
        shinyMaterial, shinyMaterial, shinyMaterial, shinyMaterial,
        shinyMaterial, shinyMaterial, shinyMaterial, shinyMaterial
    };

    void (*drawingFuncs[])(Box*, mat4, void*) = {
        NULL,   // Base
        NULL,   // Teeth

        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
    };

    MAKE_MODEL(root, model, specifications, textureMap, materialMap, drawingFuncs);

    engine->models->insert(engine->models, "trap", root, true);
}


void initSafeZone(Backend* engine, Material* shinyMaterial)
{
    Box* root = NULL;
    HashTable* textures = engine->textures;

    Texture* texture1 = (Texture*)textures->search(textures, "safe_zone");

    root = newBox((vec3){0.0f, 0.0f, 0.0f});
    root->setScale(root, (vec3){1.0f, 0.05f, 1.0f});
    memcpy(root->material, shinyMaterial, sizeof(Material));
    root->addTexture(root, texture1);

    root->setPosition(root, (vec3){engine->safeZone[X_COORD],
                                   -2.0f,
                                   engine->safeZone[Z_COORD]});
    engine->models->insert(engine->models, "safe_zone", root, true);
}


