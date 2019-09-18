#ifndef BOX_H
#define BOX_H

#include <cglm/vec3.h>

#include "list.h"
#include "texture.h"

#include "shader.h"

#define ERR_BOX_MALLOC "Error: unable to allocate memory for box\n"

typedef struct Box
{
    Shader* shader;
    List* textures;
    vec3 position;

    void (*setShader)(struct Box*, Shader*);
    void (*addTexture)(struct Box*, Texture*);
    void (*setPosition)(struct Box*, vec3);
    void (*draw)(struct Box*);
} Box;

Box* newBox(vec3);

#endif
