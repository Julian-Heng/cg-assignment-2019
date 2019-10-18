#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>
#include <cglm/io.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "macros.h"
#include "material.h"
#include "shader.h"
#include "texture.h"

#include "box.h"


static float VERTICES[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};


static void linkMethods(Box*);
static void setGlBuffers(Box*);

static void attach(Box*, Box*);

static void setShader(Box*, Shader*);
static void addTexture(Box*, Texture*);
static void setModelPosition(Box*, vec3);
static void setPosition(Box*, vec3);
static void setScale(Box*, vec3);
static void setRotation(Box*, vec3);
static void setRotationDelta(Box*, vec3);

static void recordInitialPosition(Box*);
static void recordInitialRotation(Box*);
static void resetPosition(Box*);
static void resetRotation(Box*);

static void move(Box*, vec3);
static void transformPosition(Box*, mat4);

static void draw(Box*, void*);
static void setupShader(Box*);
static void setupTexture(Box*);
static void setupModelMatrix(Box*, mat4, void*);
static void destroy(Box*);


Box* newBox(vec3 modelPosition)
{
    Box* box;

    if (! (box = (Box*)malloc(sizeof(Box))))
    {
        fprintf(stderr, ERR_BOX_MALLOC);
        return NULL;
    }

    memset(box, 0, sizeof(Box));
    linkMethods(box);

    box->textures = newList();
    box->attached = newList();

    setGlBuffers(box);
    box->setModelPosition(box, modelPosition);
    box->setScale(box, NULL);
    box->setRotation(box, NULL);
    box->recordInitialPosition(box);
    box->material = newMaterial();

    if (! box->material)
    {
        box->textures->deleteListShallow(&(box->textures));
        free(box);
        return NULL;
    }

    return box;
}


static void linkMethods(Box* this)
{
    this->attach = attach;
    this->setShader = setShader;
    this->addTexture = addTexture;
    this->setModelPosition = setModelPosition;
    this->setPosition = setPosition;
    this->setScale = setScale;
    this->setRotation = setRotation;
    this->setRotationDelta = setRotationDelta;

    this->recordInitialPosition = recordInitialPosition;
    this->recordInitialRotation = recordInitialRotation;
    this->resetPosition = resetPosition;
    this->resetRotation = resetRotation;

    this->move = move;
    this->transformPosition = transformPosition;

    this->draw = draw;
    this->setupShader = setupShader;
    this->setupTexture = setupTexture;
    this->setupModelMatrix = setupModelMatrix;
    this->destroy = destroy;
}


static void attach(Box* this, Box* attach)
{
    this->attached->insertLast(this->attached, attach, true);
}


static void setGlBuffers(Box* this)
{
    glGenVertexArrays(1, &(this->VAO));
    glGenBuffers(1, &(this->VBO));

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(VERTICES),
                 VERTICES,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


static void setShader(Box* this, Shader* shader)
{
    this->shader = shader;
}


static void addTexture(Box* this, Texture* texture)
{
    this->textures->insertLast(this->textures, texture, true);
}


static void setModelPosition(Box* this, vec3 modelPosition)
{
    ListNode* iter;
    vec3 delta;

    glm_vec3_sub(modelPosition, this->modelPosition, delta);

    glm_vec3_copy(modelPosition ? modelPosition
                                : (vec3){0.0f, 0.0f, 0.0f}, this->modelPosition);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->move((Box*)(iter->value), delta);
}


static void setPosition(Box* this, vec3 position)
{
    ListNode* iter;
    vec3 delta;

    glm_vec3_sub(position, this->position, delta);

    glm_vec3_copy(position ? position
                           : (vec3){0.0f, 0.0f, 0.0f}, this->position);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->move((Box*)(iter->value), delta);
}


static void setScale(Box* this, vec3 scale)
{
    glm_vec3_copy(scale ? scale
                        : (vec3){1.0f, 1.0f, 1.0f}, this->scale);
}


static void setRotation(Box* this, vec3 rotation)
{
    ListNode* iter;

    glm_vec3_copy(rotation ? rotation
                           : (vec3){0.0f, 0.0f, 0.0f}, this->rotation);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->setRotation((Box*)(iter->value), rotation);
}


static void setRotationDelta(Box* this, vec3 rotation)
{
    ListNode* iter;

    glm_vec3_add(rotation ? rotation
                          : (vec3){0.0f, 0.0f, 0.0f}, this->rotation,
                                                      this->rotation);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->setRotationDelta((Box*)(iter->value), rotation);
}


static void recordInitialPosition(Box* this)
{
    ListNode* iter;
    glm_vec3_copy(this->position, this->initialPosition);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->recordInitialPosition((Box*)(iter->value));
}


static void recordInitialRotation(Box* this)
{
    ListNode* iter;
    glm_vec3_copy(this->rotation, this->initialRotation);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->recordInitialRotation((Box*)(iter->value));
}


static void resetPosition(Box* this)
{
    ListNode* iter;
    this->setPosition(this, this->initialPosition);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->resetPosition((Box*)(iter->value));
}


static void resetRotation(Box* this)
{
    ListNode* iter;
    this->setRotation(this, this->initialRotation);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->resetRotation((Box*)(iter->value));
}


static void move(Box* this, vec3 delta)
{
    ListNode* iter;
    glm_vec3_add(delta, this->position, this->position);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->move((Box*)(iter->value), delta);
}


static void transformPosition(Box* this, mat4 transform)
{
    ListNode* iter;
    glm_mat4_mulv3(transform, this->position, 1.0f, this->position);

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->transformPosition((Box*)(iter->value), transform);
}


static void draw(Box* this, void* pointer)
{
    ListNode* iter;
    Box* box;
    int i = 0;

    mat4 model;

    glBindVertexArray(this->VAO);

    this->setupShader(this);
    this->setupTexture(this);
    this->setupModelMatrix(this, model, pointer);
    this->shader->setMat4(this->shader, "model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    LIST_FOR_EACH(this->attached, iter)
    {
        box = (Box*)(iter->value);
        box->setShader(box, this->shader);
        box->draw(box, pointer);
    }

    i = 0;
    LIST_FOR_EACH(this->textures, iter)
    {
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


static void setupShader(Box* this)
{
    this->shader->use(this->shader);

    this->shader->setVec3(this->shader, "material.ambient",
                          this->material->ambient);

    this->shader->setInt(this->shader, "material.diffuse",
                         this->material->diffuse);

    this->shader->setInt(this->shader, "material.specular",
                         this->material->specular);

    this->shader->setFloat(this->shader, "material.shininess",
                           this->material->shininess);
}


static void setupTexture(Box* this)
{
    ListNode* iter;
    int i = 0;

    LIST_FOR_EACH(this->textures, iter)
    {
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, ((Texture*)iter->value)->ID);
    }
}


static void setupModelMatrix(Box* this, mat4 model, void* pointer)
{
    glm_mat4_identity(model);

    glm_translate(model, this->position);

    glm_rotate_x(model, glm_rad(this->rotation[X_COORD]), model);
    glm_rotate_y(model, glm_rad(this->rotation[Y_COORD]), model);
    glm_rotate_z(model, glm_rad(this->rotation[Z_COORD]), model);

    glm_translate(model, this->modelPosition);

    glm_scale(model, this->scale);
}


static void destroy(Box* this)
{
    ListNode* iter;

    LIST_FOR_EACH(this->attached, iter)
        ((Box*)(iter->value))->destroy((Box*)(iter->value));
    this->attached->deleteList(&this->attached);
    this->textures->deleteListShallow(&this->textures);
}
