#ifndef __INPUT_H__
#define __INPUT_H__


#include "raylib.h"
#include "memory.h"

bool has_controller;

void InitInput() {
    for (s32 i = 0; i < 2000; ++i) {
        int gamepad = i;
        has_controller = IsGamepadAvailable(gamepad);
        const char *name = GetGamepadName(gamepad);
        if (has_controller) {

            printf("%d: %s (%d)\n", gamepad, name, has_controller);
        }
    }
}

bool DoMoveLeft() {
    bool key_left = IsKeyDown(KEY_LEFT);
    return key_left;
}

bool DoMoveRight() {
    bool key_right = IsKeyDown(KEY_RIGHT);
    return key_right;
}

bool DoJump() {
    bool key_space = IsKeyPressed(KEY_SPACE);
    return key_space;
}

#endif
