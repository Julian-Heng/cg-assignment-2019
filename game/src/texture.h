#ifndef TEXTURE_H
#define TEXTURE_H

#define ERR_TEXTURE_LOAD "Failed to load texture: \"%s\"\n"

typedef struct TextureSpec
{
    char* filename;
    unsigned int rgbMode;
    bool flip;
} TextureSpec;

void generateTexture(unsigned int*, TextureSpec);
void loadTexture(TextureSpec);

#endif
