#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"

#define SHADER_ERR "Error: Shader file \"%s\" failed to compile\n%s"
#define PROGRAM_ERR "Error: Shader file \"%s\" failed to link\n%s"


static unsigned int compileShader(const char*, int);
static unsigned int linkProgram(unsigned int, unsigned int);
static void checkCompile(unsigned int, int, char*);
static char* fileRead(char*);


unsigned int makeShader(char* vertexFilename, char* fragmentFilename)
{
    unsigned int ID;
    unsigned int vertex;
    unsigned int fragment;

    const char* vertexSource = fileRead(vertexFilename);
    const char* fragmentSource = fileRead(fragmentFilename);

    vertex = compileShader(vertexSource, GL_VERTEX_SHADER);
    checkCompile(vertex, SHADER, vertexFilename);

    fragment = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    checkCompile(fragment, SHADER, fragmentFilename);

    ID = linkProgram(vertex, fragment);
    checkCompile(ID, PROGRAM, vertexFilename);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free((char*)vertexSource);
    vertexSource = NULL;
    free((char*)fragmentSource);
    fragmentSource = NULL;

    return ID;
}


static unsigned int compileShader(const char* source, int type)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}


static unsigned int linkProgram(unsigned int vertex, unsigned int fragment)
{
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
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
