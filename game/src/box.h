#ifndef BOX_H
#define BOX_H

#include <cglm/vec3.h>
#include "shader.h"

#define ERR_BOX_MALLOC "Cannot allocate memory for box\n"

typedef struct Box
{
    Shader* shader;
    unsigned int texture;
    vec3 position;

    void (*setShader)(struct Box*, Shader*);
    void (*setTexture)(struct Box*, unsigned int);
    void (*setPosition)(struct Box*, vec3);
    void (*draw)(struct Box*);
} Box;

Box* newBox();

#endif
