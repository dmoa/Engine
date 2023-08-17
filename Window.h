#pragma once

#include "Graphics.h"
#include "Clock.h"

struct Window {
    void Init();
    void Clear();
    void Resized(int new_w, int new_h);
    void SetDrawGameplay();
    void SetDrawBackground();
    void SetDrawOverlay();
    void Present();
    void Shutdown();

    int desktop_w;
    int desktop_h;
    SDL_Window *window;
    SDL_GLContext gl_context = NULL;
    GLint gl_program_default;
    GLint default_framebuffer;

    Texture_Framebuffer gameplay_target;
    Texture_Framebuffer background_target;
    Texture_Framebuffer overlay_target;

    // @TODO Fix this crap
    SDL_Texture *other_texture;
    Rect other_texture_rect;
    SDL_Surface *icon;
    float ticker = 0.0;
};

#ifdef ENGINE_IMPLEMENTATION

// Blame c++ header bs, not me
#include "Graphics.h"

void Window::Init() {
    ///// OPENGL /////

    window = SDL_CreateWindow("Island Citadel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_graphics.w, g_graphics.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);

    SDL_Surface* icon_surface = SDL_LoadBMP("Assets/icon.bmp");
    if (! icon_surface) {
        print("Failed to load icon.");
    }
    SDL_SetWindowIcon(window, icon_surface);
    SDL_FreeSurface(icon_surface);


    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        print("OpenGL Context Error: %s", SDL_GetError());
    }

    if (glew_init() != GLEW_OK) {
        print("Glew failed to initialise!");
        exit(EXIT_FAILURE);
    }

    // misc
    glEnable(GL_TEXTURE_2D);

    // transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &default_framebuffer);

    // @TODO hacked together, make this less shit.
    g_graphics.gameplay_target_w = 512;
    g_graphics.gameplay_target_h = 512;
    gameplay_target = CreateTextureFramebuffer(g_graphics.gameplay_target_w, g_graphics.gameplay_target_h, default_framebuffer);
    // Overlay target is the size of the screen (* scale), but we resize it if
    // window is resized.
    overlay_target = CreateTextureFramebuffer(g_graphics.w / g_graphics.scale, g_graphics.h / g_graphics.scale, default_framebuffer);
    background_target = CreateTextureFramebuffer(g_graphics.w / g_graphics.scale, g_graphics.h / g_graphics.scale, default_framebuffer);

    // Now that we've created the window and all the opengl stuff we need,
    // we actually need to put the two together (window + opengl components).
    gl_program_default = CreateGLProgram("default.vert", "default.frag");

    g_graphics.gl_program_posteffects_gameplay = CreateGLProgram("default.vert", "default_2.frag");

    // Going to finally activate the default shaders that we wanted to use at the
    // beginning
    glUseProgram(gl_program_default);
    SetDrawOpacity(1.0);

    ///// NOT OPENGL /////

    // icon
    // icon = SDL_LoadBMP("assets/icon.bmp");
    // if (!icon) print("assets/icon.bmp not loaded");
    // SDL_SetWindowIcon(window, icon);

    SDL_GetWindowSize(window, &g_graphics.w, &g_graphics.h);

    Resized(g_graphics.w, g_graphics.h); // hack to set correct dimensions of gameplay_target
    // might not need the line below, @ CHECK
    g_graphics.framebuffer_w = g_graphics.framebuffer_h = 512;

    SDL_ShowCursor(SDL_DISABLE);
}


void Window::Clear() {
    glClearColor(0.0, 0.3, 0.5, 0.0); // navy blue, now seethrough
    glBindFramebuffer(GL_FRAMEBUFFER, gameplay_target.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0, 0.0, 0.0, 0.0); // grey
    glBindFramebuffer(GL_FRAMEBUFFER, overlay_target.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0, 0.0, 0.0, 0.0); // grey
    glBindFramebuffer(GL_FRAMEBUFFER, background_target.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.5, 0.5, 0.5, 1); // grey
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT);
}


void Window::Resized(int new_w, int new_h) {

    g_graphics.w = new_w;
    g_graphics.h = new_h;

    if (gameplay_target.texture.w * g_graphics.scale > g_graphics.w || gameplay_target.texture.h * g_graphics.scale > g_graphics.h) {
        g_graphics.scale = 2.0;
    }

    g_graphics.gameplay_target_x = (g_graphics.w - gameplay_target.texture.w * g_graphics.scale) / 2;
    g_graphics.gameplay_target_y = (g_graphics.h - gameplay_target.texture.h * g_graphics.scale) / 2;
    ResizeTextureFramebuffer(& overlay_target, new_w / g_graphics.scale, new_h / g_graphics.scale);
    ResizeTextureFramebuffer(& background_target, new_w / g_graphics.scale, new_h / g_graphics.scale);
}


void Window::SetDrawGameplay() {
    SetCurrentFramebuffer(&gameplay_target);
    glUseProgram(gl_program_default);
}


void Window::SetDrawBackground() {
    SetCurrentFramebuffer(& background_target);
    glUseProgram(gl_program_default);
}


void Window::SetDrawOverlay() {
    SetCurrentFramebuffer(& overlay_target);
    glUseProgram(gl_program_default);
}


void Window::Present() {
    glUseProgram(gl_program_default);
    SetDrawColor(1.0, 1.0, 1.0, 1.0);
    SetCurrentFramebuffer(default_framebuffer);


    glUseProgram(gl_program_default);
    SetDrawColor(1.0, 1.0, 1.0, 1.0);
    DrawTexture(background_target.texture, 0, 0, g_graphics.scale);

    glUseProgram(g_graphics.gl_program_posteffects_gameplay);
    SetDrawColor(1.0, 1.0, 1.0, 1.0);
    SendLightsToProgram(g_graphics.gl_program_posteffects_gameplay);
    DrawTexture(gameplay_target.texture, g_graphics.gameplay_target_x, g_graphics.gameplay_target_y, g_graphics.scale);

    // the gameplay window is scaled because we want to enlarge the small pixel art grid.
    // DrawTexture(gameplay_target.texture);

    glUseProgram(gl_program_default);
    SetDrawColor(1.0, 1.0, 1.0, 1.0);
    DrawTexture(overlay_target.texture, 0, 0, g_graphics.scale);

    SDL_GL_SwapWindow(window);

    ticker += g_dt / 10;
}


void Window::Shutdown() {
    FreeTextureFramebuffer(&gameplay_target);
    FreeTextureFramebuffer(&overlay_target);

    glDeleteProgram(gl_program_default);
    glDeleteProgram(g_graphics.gl_program_posteffects_gameplay);
    // Don't need to delete shaders because they'll be auto-deleted when we delete
    // the program.

    SDL_FreeSurface(icon);
    SDL_DestroyWindow(window);
}

Window window;

#endif