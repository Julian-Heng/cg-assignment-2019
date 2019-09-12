#ifndef MACROS_H
#define MACROS_H

#define IDENTITY_MAT4 \
    {{1, 0, 0, 0}, \
     {0, 1, 0, 0}, \
     {0, 0, 1, 0}, \
     {0, 0, 0, 1}}

#define RAD(angle) glm_rad((angle))

#define SAFE_FREE(p) \
    if ((p)) \
    { \
        free((p)); \
        (p) = NULL; \
    }

#endif
