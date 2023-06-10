#pragma once

#include "Engine.h"
#include "Asset.h"

struct CurAnimation {
        char *name = NULL;
        int frame_i;
        float tick;
        Rect quad;
};


Animation_Tag GetTag(Asset_Ase_Animated *asset, char *str);
void Animation_Set(CurAnimation *anim, Asset_Ase_Animated *asset, char *name); // pass name = "entire" to loop over all frames
void Animation_SetIf(CurAnimation *anim, Asset_Ase_Animated *asset, char *name);
bool Animation_Update(CurAnimation *anim, Asset_Ase_Animated *asset);
bool Animation_Update_CustomTick(CurAnimation *anim, Asset_Ase_Animated *asset, float dt);


#ifdef ENGINE_IMPLEMENTATION

Animation_Tag GetTag(Asset_Ase_Animated *asset, char *str) {

    if (strequal(str, "entire")) return {"entire", 0, asset->num_frames - 1};

    for (u16 i = 0; i < asset->tags.num_tags; i++) {
        if (strequal(asset->tags.tags[i].name, str))
            return asset->tags.tags[i];
    }
    return {"", -1, -1 };
}

void Animation_Set(CurAnimation *anim, Asset_Ase_Animated *asset, char *name) {

    Animation_Tag tag = GetTag(asset, name);

    // If animation doesn't exist, don't bother.
    // We can check if from is -1, because that is what returned from GetTag if we
    // can't find the animation.
    if (tag.from == -1) {
        print("Failed to set animation %s for asset %s\n", name, asset->file_path);
        return;
    }

    free(anim->name); // Free() will ignore if anim->name is NULL, so nothing to
                      // worry about.
    anim->name = strmalloc(name);

    anim->frame_i = tag.from;
    anim->tick = asset->frame_durations[anim->frame_i];
    anim->quad = { anim->frame_i * asset->frame_width, 0, asset->frame_width, asset->frame_height };
}

void Animation_SetIf(CurAnimation *anim, Asset_Ase_Animated *asset, char *name) {
    if (!strequal(name, anim->name))
        Animation_Set(anim, asset, name);
}


bool Animation_Update(CurAnimation *anim, Asset_Ase_Animated *asset) {
    anim->tick -= g_dt * 1000; // convert dt into milliseconds
    if (anim->tick < 0) {
        Animation_Tag t = GetTag(asset, anim->name);
        anim->frame_i = (anim->frame_i - t.from + 1) % (t.to - t.from + 1) + t.from;
        anim->tick = asset->frame_durations[anim->frame_i];

        anim->quad.x = anim->frame_i * asset->frame_width;

        // Return true if an animation cycle has finished
        if (anim->frame_i - t.from == 0)
            return true;
    }

    return false;
}

bool Animation_Update_CustomTick(CurAnimation *anim, Asset_Ase_Animated *asset, float dt) {
    anim->tick -= dt * 1000; // convert dt into milliseconds
    if (anim->tick < 0) {
        Animation_Tag t = GetTag(asset, anim->name);
        anim->frame_i = (anim->frame_i - t.from + 1) % (t.to - t.from + 1) + t.from;
        anim->tick = asset->frame_durations[anim->frame_i];

        anim->quad.x = anim->frame_i * asset->frame_width;

        // Return true if an animation cycle has finished
        if (anim->frame_i - t.from == 0)
            return true;
    }

    return false;
}

#endif