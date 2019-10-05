#ifndef MATERIAL_H
#define MATERIAL_H

#include <cglm/vec3.h>

#define ERR_MATERIAL_MALLOC "Error: unable to allocate memory for material\n"

typedef struct Material
{
    vec3 ambient;
    int diffuse;
    int specular;
    float shininess;

    void (*setAmbient)(struct Material*, vec3);
    void (*setDiffuse)(struct Material*, int);
    void (*setSpecular)(struct Material*, int);
    void (*setShininess)(struct Material*, float);
} Material;

Material* newMaterial(void);

#endif
