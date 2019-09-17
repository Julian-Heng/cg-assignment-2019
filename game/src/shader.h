#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdbool.h>

#define ERR_SHADER_MALLOC "Cannot allocate memory for shader\n"
#define ERR_SHADER "Error: Shader file \"%s\" failed to compile\n%s"
#define ERR_PROGRAM "Error: Shader file \"%s\" failed to link\n%s"

#define UNIFORM_LOC(shaderPtr, name) \
    glGetUniformLocation((shaderPtr)->ID, (name))

typedef enum {SHADER, PROGRAM} Type;

typedef struct Shader
{
    unsigned int ID;
    char vertexFilename[BUFSIZ];
    char fragmentFilename[BUFSIZ];

    void (*use)(struct Shader*);
    void (*setBool)(struct Shader*, const char*, bool);
    void (*setInt)(struct Shader*, const char*, int);
    void (*setFloat)(struct Shader*, const char*, float);
    void (*setMat4)(struct Shader*, const char*, mat4);
} Shader;

Shader* makeShader(char*, char*);

#endif
