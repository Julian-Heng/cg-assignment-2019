#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "shader.h"

#define SHADER_ERR "Error: Shader file \"%s\" failed to compile\n%s"
#define PROGRAM_ERR "Error: Shader file \"%s\" failed to link\n%s"

static void checkCompile(unsigned int, int, char*);

unsigned int makeShader(char* vertexFilename, char* fragmentFilename)
{
    unsigned int ID;
    unsigned int vertex;
    unsigned int fragment;

    const char* vertexSource = fileRead(vertexFilename);
    const char* fragmentSource = fileRead(fragmentFilename);

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    checkCompile(vertex, SHADER, vertexFilename);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    checkCompile(fragment, SHADER, fragmentFilename);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompile(ID, PROGRAM, vertexFilename);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free((char*)vertexSource);
    free((char*)fragmentSource);
    vertexSource = NULL;
    fragmentSource = NULL;

    return ID;
}


void useShader(int ID)
{
    glUseProgram(ID);
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
                fprintf(stderr, SHADER_ERR, name, info);
            }
            break;
        case PROGRAM:
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (! success)
            {
                glGetProgramInfoLog(shader, BUFSIZ, NULL, info);
                fprintf(stderr, PROGRAM_ERR, name, info);
            }
            break;
    }
}
