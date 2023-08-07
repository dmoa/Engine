#pragma once

#include <string>
#include <unordered_map>

#include "Engine.h"
#include "utils/ase_loader.h"
#include "utils/stb_image.h"

struct Tags {
        Animation_Tag *tags;
        u16 num_tags;
};

struct Asset_Texture : Texture {
        char *file_path;
};

struct Asset_Ase {
        char *file_path; // defacto name
        Texture texture;
        int frame_width;
        int frame_height;

        // If a sprite in the future does not need a boxes, it would likely
        // be an edge case and we waste an negligable amount of memory when holding
        // empty pointers.

        // For the draw order and for where an asset / entity can be damaged
        Rect *movement_box;
        // Used by the asset / entity to damage other assets / entities.
        Rect *collision_box;
};

struct Asset_Ase_Animated : Asset_Ase {
        int num_frames;
        u16 *frame_durations;
        Tags tags;
};

Asset_Texture LoadAsset_Texture(char *file_path);
Asset_Ase *LoadAsset_Ase(char* file_path, bool* is_animated = NULL);
Asset_Ase_Animated *LoadAsset_Ase_Animated(char *file_path);

void DestroyAsset_Texture(Asset_Texture image);
void DestroyAsset_Ase(Asset_Ase *a);
void DestroyAsset_Ase_Animated(Asset_Ase_Animated *a);

#ifdef ENGINE_IMPLEMENTATION

// @TODO why do we use strmalloc_wt and not strmalloc again? Write comment
// explaining

Asset_Texture LoadAsset_Texture(char *file_path) {
    int width;
    int height;
    int num_channels;
    void *pixels = stbi_load(file_path, &width, &height, &num_channels, 0);
    auto format = (num_channels == 3) ? GL_RGB : GL_RGBA;

    if (! pixels)
        print("Couldn't load %s", file_path);
    // if (format != GL_RGBA) print("%s: Warning: format is not GL_RGBA",
    // file_path);

    Texture texture = CreateTexture(width, height, pixels, format);

    return { texture.gl_texture, texture.w, texture.h, strmalloc(file_path) };
}


// @TODO, change strmalloc_wt to strmalloc
Asset_Ase *LoadAsset_Ase(char* file_path, bool* is_animated) {
    Ase_Output *output = Ase_Load(file_path);

    // output->frame_height is not multipled by num frames because frames only go
    // across in Ase_Loader, so frame_height will always be spritesheet height.
    if (output->bpp != 4) {
        print("%s: Not rgba format!", file_path);
        return NULL;
    }

    Texture texture = CreateTexture(output->frame_width * output->num_frames, output->frame_height, output->pixels);

    Asset_Ase *asset;
    if (output->num_frames > 1) {
        Asset_Ase_Animated *_asset = bmalloc(Asset_Ase_Animated);
        *_asset = { strmalloc_wt(file_path),
                    texture,
                    output->frame_width,
                    output->frame_height,
                    NULL,
                    NULL,
                    output->num_frames,
                    output->frame_durations,
                    { output->tags, output->num_tags } };
        asset = (Asset_Ase *)_asset;

    }
    else {
        asset = bmalloc(Asset_Ase);
        *asset = { strmalloc_wt(file_path), texture, output->frame_width, output->frame_height, NULL, NULL };

        free(output->tags);
        for (int i = 0; i < output->num_tags; i++) {
            free(output->tags[i].name);
        }
    }

    for (int i = 0; i < output->num_slices; i++) {
        if (strequal(output->slices[i].name, "movement_box")) {
            asset->movement_box = bmalloc(Rect);
            *(asset->movement_box) = output->slices[i].quad;
        }
        else if (strequal(output->slices[i].name, "collision_box")) {
            asset->collision_box = bmalloc(Rect);
            *(asset->collision_box) = output->slices[i].quad;
        }
        else {
            print("%s: Asset_Ase slice %s not supported", file_path, output->slices[i].name);
        }
    }

    if (is_animated != NULL) {
        *is_animated = output->num_frames > 1;
    }

    // Not using Ase_Destroy_Output() because we still want to use
    // the tags and frame durations from output. Instead, we only delete the
    // pixels and the output container because the pixel data has been copied into
    // SDL_Texture.
    for (int i = 0; i < output->num_slices; i++) {
        free(output->slices[i].name);
    }
    free(output->slices);
    free(output);

    return asset;
}


Asset_Ase_Animated *LoadAsset_Ase_Animated(char *file_path) {
    return (Asset_Ase_Animated *)LoadAsset_Ase(file_path);
}


void DestroyAsset_Texture(Asset_Texture image) {
    free(image.file_path);
    glDeleteTextures(1, & image.gl_texture);
}


void DestroyAsset_Ase(Asset_Ase *a) {
    free(a->file_path);
    FreeTexture(a->texture);
    free(a->movement_box);
    free(a->collision_box);

    free(a);
}


void DestroyAsset_Ase_Animated(Asset_Ase_Animated *a) {
    // Copying out DestroyAsset_Ase function because if we don't then we free it
    // before we can free frame_durations and tags.
    free(a->file_path);
    FreeTexture(a->texture);
    free(a->movement_box);
    free(a->collision_box);

    free(a->frame_durations);

    for (int i = 0; i < a->tags.num_tags; i++) {
        free(a->tags.tags[i].name);
    }

    free(a->tags.tags);

    free(a);
}

#endif