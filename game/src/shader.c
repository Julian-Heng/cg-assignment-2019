#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"


static void linkMethods(Shader*);

static void use(Shader*);
static void setBool(Shader*, const char*, bool);
static void setInt(Shader*, const char*, int);
static void setFloat(Shader*, const char*, float);
static void setMat4(Shader*, const char*, mat4);
static void setVec3(Shader*, const char*, vec3);

static unsigned int compileShader(char*, int);
static unsigned int linkProgram(unsigned int, unsigned int, char*);
static void checkCompile(unsigned int, int, char*);
static char* fileRead(char*);

Shader* newShader(char* vertexFilename, char* fragmentFilename)
{
    Shader* shader;
    unsigned int vertex;
    unsigned int fragment;

    if (! (shader = (Shader*)malloc(sizeof(Shader))))
    {
        fprintf(stderr, ERR_SHADER_MALLOC);
        return NULL;
    }

    memset(shader, 0, sizeof(Shader));
    linkMethods(shader);

    vertex = compileShader(vertexFilename, GL_VERTEX_SHADER);
    fragment = compileShader(fragmentFilename, GL_FRAGMENT_SHADER);

    shader->ID = linkProgram(vertex, fragment, vertexFilename);
    strncpy(shader->vertexFilename, vertexFilename, BUFSIZ);
    strncpy(shader->fragmentFilename, fragmentFilename, BUFSIZ);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shader;
}


static void linkMethods(Shader* this)
{
    this->use = use;
    this->setBool = setBool;
    this->setInt = setInt;
    this->setFloat = setFloat;
    this->setMat4 = setMat4;
    this->setVec3 = setVec3;
}


static void use(Shader* this)
{
    glUseProgram(this->ID);
}


static void setBool(Shader* this, const char* name, bool val)
{
    glUniform1i(UNIFORM_LOC(this, name), (int)val);
}


static void setInt(Shader* this, const char* name, int val)
{
    glUniform1i(UNIFORM_LOC(this, name), val);
}


static void setFloat(Shader* this, const char* name, float val)
{
    glUniform1f(UNIFORM_LOC(this, name), val);
}


static void setMat4(Shader* this, const char* name, mat4 mat)
{
    glUniformMatrix4fv(UNIFORM_LOC(this, name), 1, GL_FALSE, mat[0]);
}


static void setVec3(Shader* this, const char* name, vec3 vec)
{
    glUniform3fv(UNIFORM_LOC(this, name), 1, vec);
}


static unsigned int compileShader(char* filename, int type)
{
    const char* source = fileRead(filename);
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    checkCompile(shader, SHADER, filename);

    free((char*)source);
    source = NULL;

    return shader;
}


static unsigned int linkProgram(unsigned int vertex, unsigned int fragment,
                                char* filename)
{
    unsigned int ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompile(ID, PROGRAM, filename);

    return ID;
}


static void checkCompile(unsigned int shader, int type, char* name)
{
    int success = 0;
    char info[BUFSIZ];
    memset(info, 0, BUFSIZ);

    switch (type)
    {
        case SHADER:
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (! success)
            {
                glGetShaderInfoLog(shader, BUFSIZ, NULL, info);
                fprintf(stderr, ERR_SHADER, name, info);
            }
            break;

        case PROGRAM:
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (! success)
            {
                glGetProgramInfoLog(shader, BUFSIZ, NULL, info);
                fprintf(stderr, ERR_PROGRAM, name, info);
            }
            break;
    }
}

static char* fileRead(char* filename)
{
    char* file = NULL;
    int count = 0;
    int i = -1;
    int ch;

    FILE *fp;

    if ((fp = fopen(filename, "r")))
    {
        while ((ch = fgetc(fp)) != EOF && ! ferror(fp))
        {
            count++;
        }

        fseek(fp, 0, SEEK_SET);
        file = (char*)malloc((count + 1) * sizeof(char));
        memset(file, 0, count + 1);

        while ((ch = fgetc(fp)) != EOF && ! ferror(fp))
        {
            file[++i] = ch;
        }

        fclose(fp);
    }

    return file;
}
