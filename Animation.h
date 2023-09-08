#pragma once

#include "Engine.h"
#include "Asset.h"

struct Animation {
        std::string name = "NULL";
        int frame_i;
        float tick;
        Rect quad;
};

#define TAG_NOT_FOUND 6535

Animation_Tag GetTag(Asset_Ase_Animated *asset, std::string str);
Animation_Tag GetTag(Asset_Ase *asset, std::string str);
bool Animation_Exists(Asset_Ase_Animated* asset, std::string name);
bool Animation_Exists(Asset_Ase* asset, std::string name);
void Animation_Set(Animation *anim, Asset_Ase_Animated *asset, std::string name); // pass name = "entire" to loop over all frames
void Animation_SetIf(Animation *anim, Asset_Ase_Animated *asset, std::string name);
bool Animation_Update(Animation *anim, Asset_Ase_Animated *asset);
bool Animation_Update_CustomTick(Animation *anim, Asset_Ase_Animated *asset, float dt);



#ifdef ENGINE_IMPLEMENTATION



Animation_Tag GetTag(Asset_Ase_Animated *asset, std::string str) {

    if (str == "entire") return {"entire", 0, asset->num_frames - 1};

    for (u16 i = 0; i < asset->tags.num_tags; i++) {
        if (asset->tags.tags[i].name == str)
            return asset->tags.tags[i];
    }
    return {"", TAG_NOT_FOUND, TAG_NOT_FOUND};
}


Animation_Tag GetTag(Asset_Ase *asset, std::string str) {
    return GetTag((Asset_Ase_Animated*) asset, str);
}


bool Animation_Exists(Asset_Ase_Animated* asset, std::string name) {
    return GetTag(asset, name).from != TAG_NOT_FOUND;
}

bool Animation_Exists(Asset_Ase* asset, std::string name) {
    return Animation_Exists((Asset_Ase_Animated*) asset, name);
}


void Animation_Set(Animation *anim, Asset_Ase_Animated *asset, std::string name) {
    Animation_Tag tag = GetTag(asset, name);

    // If animation doesn't exist, don't bother.
    // We can check if from is TAG_NOT_FOUND, because that is what returned from GetTag if we
    // can't find the animation.
    if (tag.from == TAG_NOT_FOUND) {
        print("Failed to set animation %s for asset %s\n", name.c_str(), asset->file_path);
        return;
    }

    anim->name = name;
    anim->frame_i = tag.from;
    anim->tick = asset->frame_durations[anim->frame_i];
    anim->quad = { anim->frame_i * asset->frame_width, 0, asset->frame_width, asset->frame_height };
}


void Animation_SetIf(Animation *anim, Asset_Ase_Animated *asset, std::string name) {
    if (anim->name == "NULL" || name != anim->name)
        Animation_Set(anim, asset, name);
}


bool Animation_Update(Animation *anim, Asset_Ase_Animated *asset) {
    anim->tick -= SEC_PER_TICK * 1000; // convert dt into milliseconds
    if (anim->tick <= 0) {
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


bool Animation_Update_CustomTick(Animation *anim, Asset_Ase_Animated *asset, float dt) {
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