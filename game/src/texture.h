#ifndef TEXTURE_H
#define TEXTURE_H

#define ERR_TEXTURE_MALLOC "Cannot allocate memory for texture\n"
#define ERR_TEXTURE_LOAD "Failed to load texture: \"%s\"\n"

typedef struct Texture
{
    unsigned int ID;
    char filename[BUFSIZ];
} Texture;


Texture* newTexture(char* filename, unsigned int rgbMode, bool flip);

#endif
