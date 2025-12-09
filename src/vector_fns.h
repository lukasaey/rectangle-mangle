#if !defined(VECTOR_FNS_H)
#define VECTOR_FNS_H

#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#include "constants.h"

static inline Vector2 Vector2Floor(Vector2 v) {
    return (Vector2){floorf(v.x), floorf(v.y)};
}

static inline int vector_field_index(Vector2 v) {
    return roundf(v.x) + roundf(v.y) * FIELD_SIZE;
}

static inline bool vector_in_field_bounds(Vector2 v) {
    return v.x >= 0 && v.y >= 0 && v.x <= FIELD_SIZE && v.y <= FIELD_SIZE;
}

#endif // VECTOR_FNS_H
