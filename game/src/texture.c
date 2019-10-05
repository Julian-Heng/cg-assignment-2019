#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <stdbool.h>

#include "macros.h"
#include "texture.h"


Texture* newTexture(char* filename, unsigned int rgbMode, bool flip)
{
    Texture* texture;

    int width;
    int height;
    int nrChannels;
    unsigned char* data;

    if (! (texture = (Texture*)malloc(sizeof(Texture))))
    {
        fprintf(stderr, ERR_TEXTURE_MALLOC);
        return NULL;
    }

    glGenTextures(1, &(texture->ID));
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if ((data = stbi_load(filename, &width, &height, &nrChannels, 0)))
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgbMode,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_set_flip_vertically_on_load(! flip);

        strncpy(texture->filename, filename, BUFSIZ);
    }
    else
    {
        fprintf(stderr, ERR_TEXTURE_LOAD, filename);

        SAFE_FREE(texture);
        return NULL;
    }

    return texture;
}
