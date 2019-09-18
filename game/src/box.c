#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "macros.h"
#include "shader.h"
#include "texture.h"

#include "box.h"


static float VERTICES[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

static unsigned int VAO;
static unsigned int VBO;

static void linkMethods(Box*);

static void setShader(Box*, Shader*);
static void addTexture(Box*, Texture*);
static void setPosition(Box*, vec3);
static void draw(Box*);


Box* newBox(vec3 position)
{
    static bool firstRun = true;
    Box* box;

    if (! (box = (Box*)malloc(sizeof(Box))))
    {
        fprintf(stderr, ERR_BOX_MALLOC);
        return NULL;
    }

    memset(box, 0, sizeof(Box));
    linkMethods(box);

    box->textures = newList();

    if (firstRun)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(VERTICES),
                     VERTICES,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              5 * sizeof(float),
                              (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        firstRun = false;
    }

    box->setPosition(box, position);

    return box;
}


static void linkMethods(Box* this)
{
    this->setShader = setShader;
    this->addTexture = addTexture;
    this->setPosition = setPosition;
    this->draw = draw;
}


static void setShader(Box* this, Shader* shader)
{
    this->shader = shader;
}


static void addTexture(Box* this, Texture* texture)
{
    this->textures->insertLast(this->textures, texture, true);
}


static void setPosition(Box* this, vec3 position)
{
    glm_vec3_copy(position ? position : (vec3){0.0f, 0.0f, 0.0f}, this->position);
}


static void draw(Box* this)
{
    ListNode* iter = this->textures->head;
    int i = 0;

    mat4 model;

    while (iter)
    {
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, ((Texture*)iter->value)->ID);
        iter = iter->next;
    }

    glBindVertexArray(VAO);
    glm_mat4_identity(model);

    glm_translate(model, this->position);
    this->shader->setMat4(this->shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
