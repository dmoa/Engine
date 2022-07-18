#pragma once

#include "Graphics.h"
#include "Clock.h"

struct Window {
        void Init();
        void Clear();
        void Resized(int new_w, int new_h);
        void SetDrawGameplay();
        void SetDrawOverlay();
        void Present();
        void Shutdown();

        int desktop_w;
        int desktop_h;
        SDL_Window *window;
        SDL_GLContext gl_context = NULL;
        GLint gl_program_default;
        GLint gl_program_posteffects_gameplay;
        GLint default_framebuffer;

        Texture_Framebuffer gameplay_target;
        Texture_Framebuffer overlay_target;

        // @TODO Fix this crap
        SDL_Texture *other_texture;
        Rect other_texture_rect;
        SDL_Surface *icon;
};

#ifdef ENGINE_IMPLEMENTATION

// Blame c++ header bs, not me
#include "Graphics.h"

void Window::Init() {
    ///// OPENGL /////

    window = SDL_CreateWindow("juice",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              g_graphics.w,
                              g_graphics.h,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
                                  SDL_WINDOW_MAXIMIZED);

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
    print("default buff %i", default_framebuffer);

    gameplay_target = CreateTextureFramebuffer(512, 512, default_framebuffer);
    // Overlay target is the size of the screen (* scale), but we resize it if
    // window is resized.
    overlay_target = CreateTextureFramebuffer(
        g_graphics.w / g_graphics.scale, g_graphics.h / g_graphics.scale, default_framebuffer);

    // Now that we've created the window and all the opengl stuff we need,
    // we actually need to put the two together (window + opengl components).
    gl_program_default = glCreateProgram();
    gl_program_posteffects_gameplay = glCreateProgram();

    GLuint vert = LoadShader("Include/Engine/Shaders/default.vert", GL_VERTEX_SHADER);
    GLuint frag = LoadShader("Include/Engine/Shaders/default.frag", GL_FRAGMENT_SHADER);
    glAttachShader(gl_program_default, vert);
    glAttachShader(gl_program_default, frag);

    // LinkProgram(gl_program_default);
    // Doesn't actually delete them, but flags them to be deleted later.
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLuint frag_2 = LoadShader("Include/Engine/Shaders/default_2.frag", GL_FRAGMENT_SHADER);
    glAttachShader(gl_program_posteffects_gameplay, vert);
    glAttachShader(gl_program_posteffects_gameplay, frag_2);

    // LinkProgram(gl_program_posteffects_gameplay);
    glDeleteShader(frag_2);

    // Going to finally activate the default shaders that we wanted to use at the
    // beginning
    glUseProgram(gl_program_default);

    ///// NOT OPENGL /////

    // icon
    icon = SDL_LoadBMP("assets/icon.bmp");
    if (!icon)
        print("assets/icon.bmp not loaded");
    SDL_SetWindowIcon(window, icon);

    SDL_GetWindowSize(window, &g_graphics.w, &g_graphics.h);

    Resized(g_graphics.w, g_graphics.h); // hack to set correct dimensions of gameplay_target
    // might not need the line below, @ CHECK
    g_graphics.framebuffer_w = g_graphics.framebuffer_h = 512;
}

void Window::Clear() {
    glClearColor(0.0, 0.3, 0.5, 1.0); // navy blue
    glBindFramebuffer(GL_FRAMEBUFFER, gameplay_target.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0, 0.0, 0.0, 0.0); // grey
    glBindFramebuffer(GL_FRAMEBUFFER, overlay_target.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.5, 0.5, 0.5, 1); // grey
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Resized(int new_w, int new_h) {
    g_graphics.w = new_w;
    g_graphics.h = new_h;
    g_graphics.gameplay_target_x =
        (g_graphics.w - gameplay_target.texture.w * g_graphics.scale) / 2;
    g_graphics.gameplay_target_y =
        (g_graphics.h - gameplay_target.texture.h * g_graphics.scale) / 2;
    ResizeTextureFramebuffer(&overlay_target, new_w / g_graphics.scale, new_h / g_graphics.scale);
}

void Window::SetDrawGameplay() {
    SetCurrentFramebuffer(&gameplay_target);
    glUseProgram(gl_program_default);
}

void Window::SetDrawOverlay() {
    SetCurrentFramebuffer(&overlay_target);
    glUseProgram(gl_program_default);
}

void Window::Present() {
    SetCurrentFramebuffer(default_framebuffer);
    // glUseProgram(gl_program_posteffects_gameplay);
    // SendLightsToProgram(gl_program_posteffects_gameplay);

    DrawTexture(gameplay_target.texture,
                g_graphics.gameplay_target_x,
                g_graphics.gameplay_target_y,
                g_graphics.scale);

    // the gameplay window is scaled because we want to enlarge the small pixel art grid.
    // DrawTexture(gameplay_target.texture);

    glUseProgram(gl_program_default);
    DrawTexture(overlay_target.texture, 0, 0, g_graphics.scale);

    SDL_GL_SwapWindow(window);
}

void Window::Shutdown() {
    FreeTextureFramebuffer(&gameplay_target);
    FreeTextureFramebuffer(&overlay_target);

    glDeleteProgram(gl_program_default);
    glDeleteProgram(gl_program_posteffects_gameplay);
    // Don't need to delete shaders because they'll be auto-deleted when we delete
    // the program.

    SDL_FreeSurface(icon);
    SDL_DestroyWindow(window);
}

Window window;

#endif
