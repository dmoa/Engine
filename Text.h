#pragma once

#include <string>

#include "Window.h"

struct Text {
        void LoadFont(char *path = "assets/font.ttf");
        void DestroyFont();
        SDL_Texture *CreateTexture(char *text);
        TTF_Font *main_font = NULL;
        SDL_Color font_color = { 255, 255, 255 };
        const int default_font_size = 10;
};
extern Text g_text;

#ifdef ENGINE_IMPLEMENTATION

void Text::LoadFont(char *path) {
    main_font = TTF_OpenFont(path, default_font_size);
    if (!main_font)
        print("can't load font");
    else
        print("Font %s loaded.", path);
}

void Text::DestroyFont() {
    TTF_CloseFont(main_font);
}

SDL_Texture *Text::CreateTexture(char *text) {
    SDL_Surface *temp_surface = TTF_RenderText_Solid(main_font, text, font_color);
    if (!temp_surface)
        print("failed to create surface\n");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_graphics.rdr, temp_surface);

    SDL_FreeSurface(temp_surface);

    return texture;
}

Text g_text;

#endif