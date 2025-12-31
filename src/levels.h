#ifndef __LEVELS_H__
#define __LEVELS_H__


#include "entities.h"


struct CatLevel {
    const char *name;
    Entity *cat;
    Entity *portal;
    Entity *trapdoor;
    Array<Entity> entities;
};

Entity InitCatEntity(s32 frame_sz) {
    Entity cat = {};
    cat.tpe = ET_CAT;
    cat.facing_right = true;
    cat.ani_cnt = (s32) CAT_CNT;
    cat.ani_idx = 0;
    cat.ani_idx0 = 0;
    cat.ani_cnt = 0;

    cat.ani_offset = { - 15.0f * SPRITE_SCALE, - 29.0f * SPRITE_SCALE + 2 };
    cat.ani_rect = { cat.ani_offset.x, cat.ani_offset.y, (f32) frame_sz * SPRITE_SCALE, (f32) frame_sz * SPRITE_SCALE };
    cat.coll_offset = { -4.0f * SPRITE_SCALE, -18.0f * SPRITE_SCALE };
    cat.coll_rect = { cat.coll_offset.x, cat.coll_offset.y, 10 * SPRITE_SCALE, 18 * SPRITE_SCALE };

    return cat;
}

Entity InitTrapdoorEntity(s32 frame_sz) {
    Entity trapdoor = {};
    trapdoor.tpe = ET_TRAPDOOR;
    trapdoor.ani_cnt = 1;
    trapdoor.ani_idx = 0;
    trapdoor.ani_idx0 = 0;
    trapdoor.velocity = {};
    trapdoor.coll_offset = {};
    trapdoor.coll_rect = {};
    trapdoor.ani_offset = {};
    trapdoor.ani_rect = { 0, 0, (f32) frame_sz * SPRITE_SCALE, (f32) frame_sz * SPRITE_SCALE };

    return trapdoor;
}

Entity InitPortalEntity(s32 frame_sz) {
    Entity portal = {};
    portal.tpe = ET_PORTAL;

    portal.ani_cnt = 1;
    portal.ani_idx = 0;
    portal.ani_idx0 = 0;
    portal.velocity = {};
    portal.coll_offset = { - 2 + 0.5f * frame_sz * SPRITE_SCALE, - 4 + (f32) frame_sz * SPRITE_SCALE};
    portal.coll_rect = { 0, 0, 4, 4 };
    portal.ani_offset = {};
    portal.ani_rect = { 0, 0, (f32) frame_sz * SPRITE_SCALE, (f32) frame_sz * SPRITE_SCALE };

    return portal;
}

Array<Animation> LoadAnimations(MArena *a, s32 cap) {
    Array<Animation> animations = InitArray<Animation>(a, 64);
    animations.len = 1;

    animations.Add( InitAnimation("resources/1_Cat_Idle-Sheet.png", ET_CAT) );
    animations.Add( InitAnimation("resources/2_Cat_Run-Sheet.png", ET_CAT) );
    animations.Add( InitAnimation("resources/3_Cat_Jump-Sheet.png", ET_CAT) );
    animations.Add( InitAnimation("resources/4_Cat_Fall-Sheet.png", ET_CAT) );
    animations.Add( InitAnimation("resources/portal.png", ET_PORTAL) );
    animations.Add( InitAnimation("resources/trapdoor.png", ET_TRAPDOOR) );

    return animations;
}

void LoadLevelDefaults(CatLevel *level, Array<Animation> animations) {
    s32 frame_sz = animations.arr[1].frame_sz;

    level->cat = level->entities.Add( InitCatEntity(frame_sz) );
    level->portal = level->entities.Add( InitPortalEntity(frame_sz) );
    level->trapdoor = level->entities.Add( InitTrapdoorEntity(frame_sz) );

    for (s32 i = 0; i < animations.len; ++i) {
        Animation ani = animations.arr[i];

        if (ani.tpe == ET_CAT) {
            if (level->cat->ani_idx0 == 0) {
                level->cat->ani_idx0 = i;
            }
            level->cat->ani_cnt++;
        }
        else if (ani.tpe == ET_PORTAL) {
            if (level->portal->ani_idx0 == 0) {
                level->portal->ani_idx0 = i;
            }
            level->portal->ani_cnt++;
        }
        else if (ani.tpe == ET_TRAPDOOR) {
            if (level->trapdoor->ani_idx0 == 0) {
                level->trapdoor->ani_idx0 = i;
            }
            level->trapdoor->ani_cnt++;
        }
    }
}

f32 col_width = 896;
f32 grid_w = 32 * SPRITE_SCALE;
f32 grid_h = 32 * SPRITE_SCALE;
s32 grid_w_max = col_width / grid_w;

Vector2 GetGridAnchor(f32 x, f32 y) {
    Vector2 anch = { x * grid_w, y * grid_h };
    return anch;
}

void LoadColumnWalls(Array<Entity> *entities) {
    entities->Add( InitWall( { 0, -1024 }, 4056, true) );
    entities->Add( InitWall( { col_width, -1024 }, 4056, false) );
}

CatLevel LoadLevel00(MArena *a, Array<Animation> animations) {
    CatLevel level = {};
    level.entities = InitArray<Entity>(a, 64);
    LoadLevelDefaults(&level, animations);
    LoadColumnWalls(&level.entities);

    level.trapdoor->anchor = GetGridAnchor(0, 0);
    level.portal->anchor = GetGridAnchor(grid_w_max - 1, 2);

    level.entities.Add( InitPlatform( GetGridAnchor(0, 3), grid_w_max * grid_w ) );

    return level;
}

CatLevel LoadLevel01(MArena *a, Array<Animation> animations) {
    CatLevel level = {};
    level.entities = InitArray<Entity>(a, 64);
    LoadLevelDefaults(&level, animations);
    LoadColumnWalls(&level.entities);

    level.trapdoor->anchor = GetGridAnchor(0, 0);
    level.portal->anchor = GetGridAnchor(grid_w_max - 1, 2);

    level.entities.Add( InitPlatform( GetGridAnchor(0, 3), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(4.5, 3), grid_w * 2.5 ) );

    return level;
}

CatLevel LoadLevel02(MArena *a, Array<Animation> animations) {
    CatLevel level = {};
    level.entities = InitArray<Entity>(a, 64);
    LoadLevelDefaults(&level, animations);
    LoadColumnWalls(&level.entities);

    level.trapdoor->anchor = GetGridAnchor(0, 0);
    level.portal->anchor = GetGridAnchor(grid_w_max - 1, 5);

    level.entities.Add( InitPlatform( GetGridAnchor(0, 3), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(4.5, 3), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(0, 6), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(4.5, 6), grid_w * 2.5 ) );

    return level;
}

CatLevel LoadLevel03(MArena *a, Array<Animation> animations) {
    CatLevel level = {};
    level.entities = InitArray<Entity>(a, 64);
    LoadLevelDefaults(&level, animations);
    LoadColumnWalls(&level.entities);

    level.trapdoor->anchor = GetGridAnchor(0, 0);
    level.portal->anchor = { 16 * SPRITE_SCALE, 500 };

    level.trapdoor->anchor = GetGridAnchor(0, 0);
    level.portal->anchor = GetGridAnchor(0, 4);

    level.entities.Add( InitPlatform( GetGridAnchor(0, 3), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(0, 5), grid_w * 2.5 ) );

    level.entities.Add( InitPlatform( GetGridAnchor(4.5, 2), grid_w * 2.5 ) );
    level.entities.Add( InitPlatform( GetGridAnchor(4, 6.5), grid_w * 0.75 ) );

    return level;
}


#endif
