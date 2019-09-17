#ifndef TEXTURE_H
#define TEXTURE_H

#define ERR_TEXTURE_MALLOC "Error: unable to allocate memory for texture\n"
#define ERR_TEXTURE_LOAD "Error: texture \"%s\" failed to load\n"

typedef struct Texture
{
    unsigned int ID;
    char filename[BUFSIZ];
} Texture;


Texture* newTexture(char* filename, unsigned int rgbMode, bool flip);

#endif
