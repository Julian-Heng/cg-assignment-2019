#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct TextureSpec
{
    char* filename;
    unsigned int rgbMode;
    bool flip;
} TextureSpec;

void generateTexture(unsigned int*, TextureSpec);
void loadTexture(TextureSpec);

#endif
