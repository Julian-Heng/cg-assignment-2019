#ifndef BOX_H
#define BOX_H

#include <cglm/vec3.h>

#include "texture.h"
#include "shader.h"

#define ERR_BOX_MALLOC "Error: unable to allocate memory for box\n"

typedef struct Box
{
    Shader* shader;
    Texture* texture;
    vec3 position;

    void (*setShader)(struct Box*, Shader*);
    void (*setTexture)(struct Box*, Texture*);
    void (*setPosition)(struct Box*, vec3);
    void (*draw)(struct Box*);
} Box;

Box* newBox();

#endif
