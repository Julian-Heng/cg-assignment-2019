#ifndef MACROS_H
#define MACROS_H

#define SAFE_FREE(p) \
    if ((p)) \
    { \
        free((p)); \
        (p) = NULL; \
    }


#define RANGE_INC(a, b, c) \
    ((a) >= (b) && (c) >= (a))


#define MAX(a, b) \
    ((a) >= (b) ? (a) : (b))


#define MIN(a, b) \
    ((a) >= (b) ? (b) : (a))


#define ASPECT_RATIO(w, h) \
    ((float)(w) / (float)(h))

#endif
