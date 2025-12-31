#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "raylib.h"
#include "memory.h"


Vector2 Offset(Vector2 v, Vector2 offset) {
    return { v.x - offset.x, v.y - offset.y };
}

Rectangle Offset(Rectangle r, Vector2 offset) {
    return { r.x - offset.x, r.y - offset.y, r.width, r.height };
}

// DBG

void DBG_PrintFLoat(f32 value) {
    printf("%f\n", value);
}


#endif
