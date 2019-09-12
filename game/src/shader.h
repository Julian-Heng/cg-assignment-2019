#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <cglm/cglm.h>

#define ERR_SHADER "Error: Shader file \"%s\" failed to compile\n%s"
#define ERR_PROGRAM "Error: Shader file \"%s\" failed to link\n%s"

#define UNIFORM_LOC(shaderPtr, name) \
    glGetUniformLocation((shaderPtr)->ID, (name))


#define USE_SHADER(shader) glUseProgram((shader)->ID)
#define SET_SHADER_BOOL(shader, name, val) \
    glUniform1i(UNIFORM_LOC((shader), (name)), (int)(val))
#define SET_SHADER_INT(shader, name, val) \
    glUniform1i(UNIFORM_LOC((shader), (name)), (val))
#define SET_SHADER_FLOAT(shader, name, val) \
    glUniform1f(UNIFORM_LOC((shader), (name)), (val))
#define SET_SHADER_MAT4(shader, name, mat) \
    glUniformMatrix4fv(UNIFORM_LOC((shader), (name)), 1, GL_FALSE, (mat)[0])


typedef enum {SHADER, PROGRAM} Type;

typedef struct Shader
{
    unsigned int ID;
    char vertexFilename[BUFSIZ];
    char fragmentFilename[BUFSIZ];
} Shader;

Shader* makeShader(char*, char*);
void useShader(Shader*);

#endif
