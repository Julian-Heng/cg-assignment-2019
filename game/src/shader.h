#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>

typedef enum {SHADER, PROGRAM} type;

unsigned int makeShader(char*, char*);
void useShader(int);

void setShaderBool(int, char*, bool);
void setShaderInt(int, char*, int);
void setShaderFloat(int, char*, float);

#endif
