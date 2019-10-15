#ifndef MACROS_H
#define MACROS_H

#define SAFE_FREE(p) \
    if ((p)) \
    { \
        free((p)); \
        (p) = NULL; \
    }


#define RANGE_INC(a, b, c) ((a) >= (b) && (c) >= (a))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) >= (b) ? (b) : (a))
#define ASPECT_RATIO(w, h) ((float)(w) / (float)(h))
#define KEY_PRESSED(window, key) glfwGetKey((window), (key)) == GLFW_PRESS


#define MAKE_MODEL(root, model, spec, text, mat) \
    for (int i = 0; i < sizeof((spec)) / sizeof((spec)[0]); i++) \
    { \
        (model) = newBox((spec)[i][0]); \
        (model)->setScale((model), (spec)[i][1]); \
        memcpy((model)->material, (mat)[i], sizeof(Material)); \
        (model)->addTexture((model), (text)[i]); \
        if ((root)) \
        { \
            (root)->attach((root), (model)); \
        } \
        else \
        { \
            (root) = (model); \
        } \
    }

#endif
