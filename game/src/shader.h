#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>

typedef enum {SHADER, PROGRAM} type;

typedef struct Shader
{
    unsigned int ID;
    char vertexFilename[BUFSIZ];
    char fragmentFilename[BUFSIZ];
    bool success;
} Shader;

Shader* makeShader(char*, char*);
void useShader(Shader*);

void setShaderBool(Shader*, char*, bool);
void setShaderInt(Shader*, char*, int);
void setShaderFloat(Shader*, char*, float);

#endif
