#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"

#define ERR_SHADER "Error: Shader file \"%s\" failed to compile\n%s"
#define ERR_PROGRAM "Error: Shader file \"%s\" failed to link\n%s"


static unsigned int compileShader(char*, int);
static unsigned int linkProgram(unsigned int, unsigned int, char*);
static void checkCompile(unsigned int, int, char*);
static char* fileRead(char*);


unsigned int makeShader(char* vertexFilename, char* fragmentFilename)
{
    unsigned int ID;
    unsigned int vertex;
    unsigned int fragment;

    vertex = compileShader(vertexFilename, GL_VERTEX_SHADER);
    fragment = compileShader(fragmentFilename, GL_FRAGMENT_SHADER);
    ID = linkProgram(vertex, fragment, vertexFilename);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
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


static unsigned int
linkProgram(unsigned int vertex, unsigned int fragment, char* filename)
{
    unsigned int ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompile(ID, PROGRAM, filename);

    return ID;
}


void useShader(int ID)
{
    glUseProgram(ID);
}


void setShaderBool(int ID, char* name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}


void setShaderInt(int ID, char* name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}


void setShaderFloat(int ID, char* name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name), value);
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
