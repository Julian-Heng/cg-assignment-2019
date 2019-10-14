#ifndef BOX_H
#define BOX_H

#include <cglm/vec3.h>
#include <cglm/mat4.h>

#include "list.h"
#include "texture.h"
#include "material.h"

#include "shader.h"

#define ERR_BOX_MALLOC "Error: unable to allocate memory for box\n"

typedef struct Box
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int vertexAttribPointerIndex;

    Shader* shader;
    List* textures;
    vec3 modelPosition;
    vec3 position;
    vec3 scale;
    vec3 rotation;

    Material* material;

    vec3 initialPosition;
    vec3 initialRotation;

    List* attached;

    bool rotateLast;

    void (*attach)(struct Box*, struct Box*);

    void (*setShader)(struct Box*, Shader*);
    void (*addTexture)(struct Box*, Texture*);
    void (*setModelPosition)(struct Box*, vec3);
    void (*setPosition)(struct Box*, vec3);
    void (*setScale)(struct Box*, vec3);
    void (*setRotation)(struct Box*, vec3);
    void (*setRotateLast)(struct Box*, bool);

    void (*recordInitialPosition)(struct Box*);
    void (*recordInitialRotation)(struct Box*);
    void (*resetPosition)(struct Box*);
    void (*resetRotation)(struct Box*);

    void (*move)(struct Box*, vec3);
    void (*transformPosition)(struct Box*, mat4);

    void (*draw)(struct Box*);
    void (*destroy)(struct Box*);
} Box;

Box* newBox(vec3);

#endif
