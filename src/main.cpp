#include "raylib.h"

#include "memory.h"
#include "input.h"
#include "entities.h"
#include "helpers.h"
#include "levels.h"


#define ARENA_CAP 1024*1024 
u8 mem[ARENA_CAP];


enum GameState {
    GS_TITLESCREEN,
    GS_ENDSCREEN,
    GS_GAME,
    GS_TRANSITION,

    GS_CNT,
};

struct CatGame {
    GameState state;
    s32 level_at;
    s32 level_next;
    CatLevel *level;
    Array<CatLevel> levels;
    bool dbg_draw;
    Color tint;

    f32 transition_elapsed;
    f32 transition_time;

    void SetTransition(s32 to_level) {
        if (to_level == levels.len) {
            level_next = -1;
        }
        else {
            level_next = to_level % levels.len;
        }
        transition_elapsed = 0;
        state = GS_TRANSITION;
    }

    void SetTransitionToNext() {
        SetTransition(level_at + 1);
    }

    void SetLevel(s32 to_level) {
        if (to_level == -1) {
            state = GS_ENDSCREEN;
        }
        else {
            assert(to_level < levels.len);

            level_at = to_level;
            level = levels.arr + level_at;

            level->cat->anchor = GetGridAnchor(0.5f, 1);
            level->cat->velocity = {};

            Update(0);
        }
    }
    void GoToNextLevel() {
        SetLevel(level_at + 1);
    }
    void Update(f32 dt) {
        for (s32 i = 0; i < level->entities.len; ++i) {
            Entity *ent = level->entities.arr + i;
            if (ent->tpe == ET_UNKNOWN) {
                continue;
            }

            ent->Update(dt);
        }
    }
};

CatGame CatGameInit(MArena *a) {
    CatGame cg = {};

    cg.levels = InitArray<CatLevel>(a, 32);
    cg.level_at = 0;
    cg.transition_elapsed = 0;
    cg.transition_time = 300;
    cg.tint = WHITE;

    return cg;
}

CatGame game;
Camera2D cam;
Array<Animation> animations;

void DrawGame() {
    BeginDrawing();
    BeginMode2D(cam);
    ClearBackground(BLACK);

    Color color = game.tint;
    for (s32 i = 0; i < game.level->entities.len; ++i) {
        Entity *ent = game.level->entities.arr + i;
        if (ent->tpe == ET_UNKNOWN || ent->tpe == ET_CAT) {
            continue;
        }

        Frame frame = ent->GetFrame(animations);
        DrawTexturePro(frame.tex, frame.source, ent->ani_rect, cam.offset, 0.0f, color);

        if (ent->tpe == ET_PLATFORM) {
            Vector2 right = { ent->anchor.x + ent->coll_rect.width, ent->anchor.y };
            DrawLineEx( Offset(ent->anchor, cam.offset), Offset(right, cam.offset), 2, color);
        }
        else if (ent->tpe == ET_WALL_LEFT || ent->tpe == ET_WALL_RIGHT) {
            Vector2 bottom = { ent->anchor.x, ent->anchor.y + ent->coll_rect.height };
            DrawLineEx( Offset(ent->anchor, cam.offset), Offset(bottom, cam.offset), 2, color);
        }
    }

    Frame frame = game.level->cat->GetFrame(animations);
    DrawTexturePro(frame.tex, frame.source, game.level->cat->ani_rect, cam.offset, 0.0f, color);

    // DBG
    if (IsKeyPressed(KEY_TAB)) {
        game.dbg_draw = !game.dbg_draw;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        game.SetTransitionToNext();
    }
    if (game.dbg_draw) {
        for (s32 i = 0; i < game.level->entities.len; ++i) {
            Entity *ent = game.level->entities.arr + i;
            if (ent->tpe == ET_UNKNOWN) {
                continue;
            }

            ent->DrawWireframes_DBG(cam.offset);
        }
    }

    EndMode2D();
    EndDrawing();
}

void DrawTextCenterX(const char* text, s32 fontsize, s32 offset_y) {
    s32 sz = MeasureText(text, fontsize);

    DrawText(text, (GetScreenWidth() - sz) / 2, GetScreenHeight() / 2 + offset_y, fontsize, WHITE);
}


void OnWindowResize() {
    s32 window_w = GetScreenWidth();

    cam.offset = { (window_w - col_width/2) / 2 / cam.zoom, 0 / cam.zoom };
}

int main(void) {
    MArena a_life = ArenaCreate(mem, ARENA_CAP);

    // raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Cat Jump Quick");
    s32 def_w = 800;
    s32 def_h = 600;
    bool start_in_fullscreen = false;
    if (start_in_fullscreen) {
        ToggleFullscreen();
    }
    else {
        SetWindowSize(def_w, def_h);
    }

    SetTargetFPS(60);
    f32 dt = 0;
    InitInput();

    // global state
    animations = LoadAnimations(&a_life, 64);

    game = CatGameInit(&a_life);
    game.levels.Add( LoadLevel00(&a_life, animations) );
    game.levels.Add( LoadLevel01(&a_life, animations) );
    game.levels.Add( LoadLevel02(&a_life, animations) );
    game.levels.Add( LoadLevel03(&a_life, animations) );
    game.levels.Add( LoadLevel04(&a_life, animations) );
    game.levels.Add( LoadLevel05(&a_life, animations) );
    game.levels.Add( LoadLevel06(&a_life, animations) );
    game.levels.Add( LoadLevel07(&a_life, animations) );
    game.levels.Add( LoadLevel08(&a_life, animations) );

    game.SetLevel(0);
    game.state = GS_TITLESCREEN;

    cam.zoom = 0.5f;
    OnWindowResize();

    // DBG
    //game.state = GS_GAME;
    //game.SetLevel(8);

    // loop
    while (!WindowShouldClose()) {
        dt = GetFrameTime() * 1000;

        // why is this so verbose?
        if (IsKeyPressed(KEY_F10)) {
            if (IsWindowFullscreen()) {
                OnWindowResize();
                SetWindowSize(def_w, def_h);
                ToggleFullscreen();
            }
            else {
                SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                OnWindowResize();
                ToggleFullscreen();
            }
        }
        if (IsWindowResized()) {
            OnWindowResize();
        }

        if (game.state == GS_TITLESCREEN) {
            if (IsKeyPressed(KEY_SPACE)) {
                game.state = GS_GAME;
            }

            BeginDrawing();

            ClearBackground(BLACK);

            DrawTextCenterX("CAT - QUICK", 36, - 36);
            DrawTextCenterX("Press [space] to jump", 24, 24);

            EndDrawing();
        }

        if (game.state == GS_ENDSCREEN) {
            if (IsKeyPressed(KEY_SPACE)) {
                break;
            }

            BeginDrawing();

            ClearBackground(BLACK);
            DrawTextCenterX("THE END", 36, -36);

            EndDrawing();
        }

        else if (game.state == GS_GAME) {
            // fade in
            if (game.transition_elapsed < game.transition_time) {
                game.tint.a = game.transition_elapsed / game.transition_time * 255;
                game.transition_elapsed += dt;
            }
            else {
                game.tint.a = 255;
            }

            bool cat_exit = false;
            bool cat_fall = false;
            CatUpdate(game.level->cat, dt, game.level->entities, &cat_fall, &cat_exit);

            if (cat_exit) {
                game.SetTransitionToNext();
                continue;
            }

            if (cat_fall) {
                s32 to = 0;
                if (game.level_at > 0) {
                    to = game.level_at - 1;
                }
                game.SetTransition(to);
                continue;
            }

            game.Update(dt);
            DrawGame();
        }

        else if (game.state == GS_TRANSITION) {
            if (game.transition_elapsed >= game.transition_time) {
                game.state = GS_GAME;
                game.transition_elapsed = 0;
                game.SetLevel(game.level_next);
            }
            else {
                game.transition_elapsed += dt;
                game.tint.a = (game.transition_time - game.transition_elapsed) / game.transition_time * 255;
            }

            // NOTE: weirdly, this is required to elapse the time
            DrawGame();
        }

        // display the frame rate
        //DrawText(TextFormat("FRAME RATE: %0.2f FPS", 1000.0f/dt), 10, 10, 10, DARKGRAY);
    }

    UnloadTextures(animations);
    CloseWindow();
}
