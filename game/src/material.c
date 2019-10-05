#include <cglm/vec3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "material.h"


static void linkMethods(Material*);

static void setAmbient(Material*, vec3);
static void setDiffuse(Material*, int);
static void setSpecular(Material*, int);
static void setShininess(Material*, float);


Material* newMaterial()
{
    Material* mat;

    if (! (mat = (Material*)malloc(sizeof(Material))))
    {
        fprintf(stderr, ERR_MATERIAL_MALLOC);
        return NULL;
    }

    memset(mat, 0, sizeof(Material));
    linkMethods(mat);

    return mat;
}


static void linkMethods(Material* this)
{
    this->setAmbient = setAmbient;
    this->setDiffuse = setDiffuse;
    this->setSpecular = setSpecular;
    this->setShininess = setShininess;
}


static void setAmbient(Material* this, vec3 ambient)
{
    glm_vec3_copy(ambient, this->ambient);
}


static void setDiffuse(Material* this, int diffuse)
{
    this->diffuse = diffuse;
}


static void setSpecular(Material* this, int specular)
{
    this->specular = specular;
}


static void setShininess(Material* this, float shininess)
{
    this->shininess = shininess;
}
