#ifndef __ENTITIES_H__
#define __ENTITIES_H__


#include "raylib.h"
#include "memory.h"
#include "helpers.h"


#define MAX_ANIMATIONS 4
#define SPRITE_SCALE 4
#define MAX_ANIMATION_FRAMES 16


enum EntityType {
    ET_UNKNOWN,

    ET_PLATFORM,
    ET_WALL_LEFT,
    ET_WALL_RIGHT,
    ET_PORTAL,
    ET_TRAPDOOR,
    ET_CAT,

    ET_CNT
};

struct Frame {
    Rectangle source;
    s32 duration;
    Texture tex; // const

    Rectangle InvertedLeftRight() {
        Rectangle flipped = source;
        flipped.width = -flipped.width;
        flipped.x = source.x + source.width;
        return flipped;
    }

    Frame Mirror() {
        Frame flipped = *this;
        flipped.source.width = - flipped.source.width;
        return flipped;
    }
};

struct Animation {
    EntityType tpe;
    Texture texture;
    s32 frame_cnt;
    s32 frame_sz;
    Frame frames[MAX_ANIMATION_FRAMES];
};

Animation InitAnimation(const char* anifile, EntityType tpe) {
    Animation ani = {};

    ani.tpe = tpe;
    ani.texture = LoadTexture(anifile);
    assert(ani.texture.width % ani.texture.height == 0);
    ani.frame_cnt = ani.texture.width / ani.texture.height;
    ani.frame_sz = ani.texture.height;

    for (s32 i = 0; i < ani.frame_cnt; ++i) {
        ani.frames[i].source = { (f32) ani.texture.height * i, 0.0f, (f32) ani.texture.height, (f32) ani.texture.height };
        ani.frames[i].duration = 100;
        ani.frames[i].tex = ani.texture;
    }
    return ani;
}

struct EntityInterface {
    virtual void Update(f32 dt) = 0;
    virtual Frame GetFrame(Texture *texture) = 0;
};

struct Entity {
    EntityType tpe;
    s32 facing_right;
    s32 state;

    // kinematics
    Vector2 anchor;
    Vector2 velocity;
    Rectangle coll_rect;
    Vector2 coll_offset;

    // animations
    Rectangle ani_rect;
    Vector2 ani_offset;
    s32 ani_idx;
    s32 ani_idx0;
    s32 ani_cnt;
    s32 frame_idx;
    f32 frame_elapsed;

    void Update(f32 dt) {
        anchor.x += dt * velocity.x;
        anchor.y += dt * velocity.y;

        ani_rect.x = anchor.x + ani_offset.x;
        ani_rect.y = anchor.y + ani_offset.y;

        coll_rect.x = anchor.x + coll_offset.x;
        coll_rect.y = anchor.y + coll_offset.y;
    }

    Frame GetFrame(Array<Animation> animations) {
        Animation ani = animations.arr[ani_idx + ani_idx0];
        Frame frame = ani.frames[frame_idx];

        if (frame.duration == 0) {
            return frame;
        }

        if (frame_elapsed > frame.duration) {
            frame_elapsed = 0;
            frame_idx = (frame_idx + 1) % ani.frame_cnt;
            frame = ani.frames[frame_idx];
        }

        if (facing_right) {
            return frame;
        }
        else {
            return frame.Mirror();
        }
    }

    void DrawWireframes_DBG(Vector2 offset) {
        DrawRectangleLinesEx( Offset(ani_rect, offset), 2, WHITE);
        DrawRectangleLinesEx( Offset(coll_rect, offset), 4, BLUE);
        DrawRectangleLinesEx( Rectangle{ Offset(anchor, offset).x, Offset(anchor, offset).y, 2, 2 }, 2, RED);
    }
};


// cat entity

enum CatState {
    CAT_IDLE,
    CAT_RUN,
    CAT_JUMP,
    CAT_FALL,

    CAT_CNT,
};

#define CAT_RUN_SPEED 0.15f * SPRITE_SCALE
#define CAT_JUMP_SPEED 0.29f * SPRITE_SCALE
#define CAT_JUMP_BRAKE_MULT 0.3f * SPRITE_SCALE
#define CAT_FALL_ACCEL 0.014f * SPRITE_SCALE

bool CollidePlatform(Entity cat, f32 delta_y, Rectangle rect) {
    Rectangle cr = cat.coll_rect;
    if (delta_y > 0) {
        Rectangle next = { cr.x, cr.y + cr.height, cr.width, delta_y };
        return CheckCollisionRecs(next, rect);
    }
    else if (delta_y < 0) {
        return false;
    }
    else {
        return CheckCollisionRecs(cr, rect);
    }
}

bool CollideWall(Entity cat, f32 delta_x, Entity wall) {
    Rectangle rect = wall.coll_rect;
    Rectangle cr = cat.coll_rect;
    Rectangle next = {};

    if (CheckCollisionRecs(cr, rect)) {
        return true;
    }
    else if ((wall.tpe == ET_WALL_LEFT) && (delta_x < 0)) {
        next = { cr.x + delta_x, cr.y, - delta_x, cr.height };
        return CheckCollisionRecs(next, rect);
    }
    else if ((wall.tpe == ET_WALL_RIGHT) && (delta_x > 0)) {
        next = { cr.x + cr.width, cr.y, delta_x, cr.height };
        bool coll = CheckCollisionRecs(next, rect);
        return coll;
    }
    return false;
}

bool CollidePortal(Entity cat, Vector2 delta, Rectangle rect) {
    Rectangle cr = cat.coll_rect;
    Vector2 next = { cr.x + delta.x, cr.y + delta.y };
    return CheckCollisionPointRec(next, rect) || CheckCollisionRecs(cr, rect);
}

void CatUpdate(Entity *cat, f32 dt, Array<Entity> entities, bool *out_fall, bool *out_exit) {
    bool key_left = DoMoveLeft();
    bool key_right = DoMoveRight();
    bool key_space = DoJump();

    if (cat->anchor.y > 2056) {
        *out_fall = true;
        return;
    }

    if (key_right && !key_left) {
        cat->facing_right = true;
        cat->velocity.x = CAT_RUN_SPEED;
    }
    else if (key_left && !key_right) {
        cat->facing_right = false;
        cat->velocity.x = - CAT_RUN_SPEED;
    }
    else {
        cat->velocity.x = 0;
    }

    bool did_collide = false;
    for (s32 i = 0; i < entities.len; ++i) {
        Entity ent = entities.arr[i];

        if (ent.tpe == ET_PLATFORM && !did_collide) {
            did_collide = CollidePlatform(*cat, dt * cat->velocity.y, ent.coll_rect);

            if (did_collide) {
                cat->velocity.y = 0;
                cat->anchor.y = ent.anchor.y + 1;
            }
        }
        else if (ent.tpe == ET_WALL_LEFT) {
            bool did_collide_wall_left = CollideWall(*cat, dt * cat->velocity.x, ent);
            if (did_collide_wall_left ) {
                cat->velocity.x = 0;
                cat->anchor.x = ent.anchor.x + cat->coll_rect.width / 2 - 2;
            }
        }
        else if (ent.tpe == ET_WALL_RIGHT) {
            bool did_collide_wall_left = CollideWall(*cat, dt * cat->velocity.x, ent);
            if (did_collide_wall_left ) {
                cat->velocity.x = 0;
                cat->anchor.x = ent.anchor.x - cat->coll_rect.width / 2 - 5;
            }
        }
        else if (ent.tpe == ET_PORTAL) {
            bool did_collide_portal = CollidePortal(*cat, { dt * cat->velocity.x, dt * cat->velocity.y }, ent.coll_rect);
            if (did_collide_portal) {
                *out_exit = true;
                return;
            }
        }
    }
    if (did_collide == false) {
        cat->velocity.y += CAT_FALL_ACCEL;
    }
    else {
        // can only jump from a platform
        if (key_space) {
            cat->velocity.y = -1.0f * CAT_JUMP_SPEED;
            cat->anchor.y += -2;
        }
    }

    CatState set_state = CAT_IDLE;
    if (did_collide == true) {
        if (cat->velocity.x != 0.0f) {
            set_state = CAT_RUN;
        }
    }
    else {
        if (cat->velocity.y <= 0) {
            set_state = CAT_JUMP;
        }
        else {
            set_state = CAT_FALL;
        }
    }

    if (set_state != cat->state) {
        cat->state = set_state;
        cat->ani_idx = cat->state;
        cat->frame_idx = 0;
        cat->frame_elapsed = 0;
    }
    else {
        cat->frame_elapsed += dt;
    }
}

void UnloadTextures(Array<Animation> animations) {
    for (s32 i = 0; i < animations.len; ++i) {
        UnloadTexture(animations.arr[i].texture);
    }
}

Entity InitPlatform(Vector2 position, f32 width) {
    Entity platform = {};
    platform.tpe = ET_PLATFORM;
    platform.anchor = position;
    platform.coll_rect = {0, 0, width, 2};

    f32 height = 50;
    platform.ani_rect = platform.coll_rect;
    platform.ani_rect.height = height;

    return platform;
}

Entity InitWall(Vector2 position, f32 height, bool is_left) {
    Entity platform = {};
    platform.tpe = ET_WALL_RIGHT;
    if (is_left) {
        platform.tpe = ET_WALL_LEFT;
    }
    platform.anchor = position;
    platform.coll_rect = { platform.anchor.x, platform.anchor.y, 2, height };

    f32 width = 50;
    platform.ani_rect = platform.coll_rect;
    platform.ani_rect.height = width;

    return platform;
}

#endif
