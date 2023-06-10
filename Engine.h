// Engine is built on top of SDL.
// SDL functions have prefix SDL_.....
// Engine functions don't have a prefix.

#pragma once

#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <time.h>

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

struct v2 {
        float x, y;
};

struct v2i {
        int x, y;
        bool operator==(const v2i& other) const {
            return x == other.x && y == other.y;
        }
};


struct v3 {
        float x, y, z;
};

struct v4 {
        float x, y, z, w;
};

struct IntRect {
        int x, y, w, h;
};

struct FloatRect {
        float x, y, w, h;
};

// Generally avoid using.
#define Rect IntRect

// bmalloc with auto casting
// We don't use new because new usually implies a constructor.
#define bmalloc(t) (t *)(malloc(sizeof(t)))
#define bmalloc_arr(t, n) (t *)(malloc(sizeof(t) * n))

#include "utils/itoa.h"
#include "utils/string.h"

#include <GL/glew.h>

// SDL cross platform includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// printf + SDL does not work (something to do with SDL redirecting the entry
// point, I think), and so you are forced to use print. I've added a cheat here
// so that it's more readable. If I could avoid it I would.

// This also means if I ever figure out how to actually use printf, I can
// smoothly transition by doing something like #define print as printf + "\n"

#undef print
#undef printf

#define print SDL_Log
// for any libraries so that I don't have to replace all the printfs with print
// / print.
#define printf SDL_Log

// Because consistent casing is nice
#define glew_init glewInit

#ifdef _WIN32
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#elif __linux__
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#else
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef ENGINE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLT_IMPLEMENTATION
#define ASE_LOADER_IMPLEMENTATION
#endif

#include "Window.h"
#include "Graphics.h"
#include "Controls.h"
#include "Asset.h"
#include "Clock.h"
#include "Animation.h"
#include "ExtraMath.h"
#include "utils/gltext.h"

void EngineInit();
void EngineQuit();

#ifdef ENGINE_IMPLEMENTATION

void EngineInit() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    srand(time(0));
    g_controls.Init();

    Ase_SetFlipVerticallyOnLoad(true);
    stbi_set_flip_vertically_on_load(true);
}

void EngineQuit() {
    gltTerminate();
    IMG_Quit();
    SDL_Quit();
}

#endif