#ifndef SHADER_H
#define SHADER_H

typedef enum {SHADER, PROGRAM} type;

unsigned int makeShader(char*, char*);
void useShader(int);

#endif
