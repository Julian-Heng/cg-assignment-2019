#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <stdbool.h>

#include "texture.h"


void generateTexture(unsigned int* texture, TextureSpec spec)
{
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    loadTexture(spec);
}


void loadTexture(TextureSpec spec)
{
    int width;
    int height;
    int nrChannels;

    unsigned char* data;

    if ((data = stbi_load(spec.filename, &width, &height, &nrChannels, 0)))
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, spec.rgbMode,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_set_flip_vertically_on_load(!spec.flip);
    }
    else
    {
        fprintf(stderr, ERR_TEXTURE_LOAD, spec.filename);
    }
}
