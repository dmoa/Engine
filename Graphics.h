// fyi: I hate opengl with a fiery passion
// Updated: I still hate opengl.
#pragma once

#include <string>

#include "Window.h"
#include "utils/LoadFile.h"
#include "utils/ase_loader.h"

#define MAX_NUM_LIGHTS 64

struct Light {
        v2 position; // normalised
        v3 color;    // 0->1
        float power;
};

struct GlobalGraphicsData {
        int w; // window width
        int h; // window height
        int scale;
        SDL_Renderer *rdr;

        // Current framebuffer dimensions
        int framebuffer_w;
        int framebuffer_h;

        int num_lights = 0;
        Light lights[MAX_NUM_LIGHTS];
        // where gameplay is drawn from
        int gameplay_target_x;
        int gameplay_target_y;
};

extern GlobalGraphicsData g_graphics;

struct Texture {
        GLuint gl_texture;
        int w;
        int h;
};

struct Texture_Framebuffer {
        Texture texture;
        GLuint framebuffer;
};

GLuint CreateEmptyTexture(int width, int height);
Texture CreateTexture(int width, int height, void *pixels, GLenum format = GL_RGBA);
void FreeTexture(Texture texture);
void DrawTexture(Texture texture, int x = 0, int y = 0, int scale = 1);
void DrawTextureStretched(Texture texture, int x, int y, int w, int h);
// pivot is relative
void DrawTextureEx(Texture texture,
                   int x,
                   int y,
                   int source_x,
                   int source_y,
                   int source_w,
                   int source_h,
                   int scale = 1,
                   bool flip_horizontally = false,
                   int pivot_x = -1,
                   int pivot_y = -1,
                   float angle = 0);

// If source NULL, entire texture is used.
// If pivot_point NULL, it rotates about the center of the texture.
void DrawTextureEx(Texture texture,
                   int x,
                   int y,
                   Rect *source,
                   int scale = 1,
                   bool flip_horizontally = false,
                   v2 *pivot_point = NULL,
                   float angle = 0);

Texture_Framebuffer CreateTextureFramebuffer(int w, int h);
void ResizeTextureFramebuffer(Texture_Framebuffer *texture_framebuffer, int w, int h);
void FreeTextureFramebuffer(Texture_Framebuffer *);
void SetCurrentFramebuffer(Texture_Framebuffer *texture_framebuffer); // for drawing, otherwise just
                                                                      // use glBindFramebuffer

GLuint LoadShader(char *path, GLenum shader_type);
void LinkProgram(GLuint gl_program);

void PrintGLError();

int AddLight(Light light); // returns index of new light
void SendLightsToProgram(GLint current_gl_program);

#ifdef ENGINE_IMPLEMENTATION

#include "ExtraMath.h"
#include "Window.h"

float gl_window_x(float x) {
    return x / g_graphics.framebuffer_w * 2 - 1;
}

// For some reason we need to cast the integer variables but not 2 or 1. *shrug*
float gl_window_y(float y) {
    return 1 - (2 * (float)y / (float)g_graphics.framebuffer_h);
}

// We don't need to use glActiveTexture because we're not actually blending
// anything (I know, very basic)
GLuint CreateEmptyTexture(int width, int height) {
    GLuint texture;
    // Note for self: glGenTextures can generate multiple textures, but we just
    // want one, hence the 1. Update, maybe it's in reference to glGenTextures? So
    // if I generate 80 textures, then GL_TEXTURE0 is the 1st texture in the 80
    // generated, GL_TEXTURE20 is the 19th texture generated and so on? I think
    // this is it but I honestly don't know, OpenGL Documentation sucks.

    glActiveTexture(GL_TEXTURE0); // Does this line even need to be here? @ Question ->
                                  // apparently yes, but no clue why :/
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Lots of defaults here, but setting them just in case
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Creating the texture with format RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    return texture;
};

// Default input format is RGBA
Texture CreateTexture(int width, int height, void *pixels, GLenum format) {
    Texture texture;
    texture.w = width;
    texture.h = height;

    // Note for self: glGenTextures can generate multiple textures, but we just
    // want one, hence the 1.
    glGenTextures(1, &texture.gl_texture);
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture);

    // Lots of defaults here, but setting them just in case
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    free(pixels);

    return texture;
};

void FreeTexture(Texture texture) {
    glDeleteTextures(1, &texture.gl_texture);
};

void DrawTexture(Texture texture, int x, int y, int scale) {
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
    glBegin(GL_QUADS);

    float left = gl_window_x(x);
    float right =
        gl_window_x(x + texture.w * scale); // (-) instead of (+) because opengl coordinate system
    float top = gl_window_y(y);
    float bottom = gl_window_y(y + texture.h * scale);

    glTexCoord2f(0.0, 1.0);
    glVertex2f(left, top); // Top Left Corner
    glTexCoord2f(1.0, 1.0);
    glVertex2f(right, top); // Top Right Corner
    glTexCoord2f(1.0, 0.0);
    glVertex2f(right, bottom); // Bottom Right Corner
    glTexCoord2f(0.0, 0.0);
    glVertex2f(left, bottom); // Bottom Left Corner

    glEnd();
}

void DrawTextureStretched(Texture texture, int x, int y, int w, int h) {
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
    glBegin(GL_QUADS);

    float left = gl_window_x(x);
    float right = gl_window_x(x + w); // (-) instead of (+) because opengl coordinate system
    float top = gl_window_y(y);
    float bottom = gl_window_y(y + h);

    glTexCoord2f(0.0, 1.0);
    glVertex2f(left, top); // Top Left Corner
    glTexCoord2f(1.0, 1.0);
    glVertex2f(right, top); // Top Right Corner
    glTexCoord2f(1.0, 0.0);
    glVertex2f(right, bottom); // Bottom Right Corner
    glTexCoord2f(0.0, 0.0);
    glVertex2f(left, bottom); // Bottom Left Corner

    glEnd();
}

void DrawTextureEx(Texture texture,
                   int x,
                   int y,
                   int source_x,
                   int source_y,
                   int source_w,
                   int source_h,
                   int scale,
                   bool flip_horizontally,
                   int pivot_x,
                   int pivot_y,
                   float angle) {
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
    glBegin(GL_QUADS);

    // Texture Coords

    float tex_left = (float)source_x / (float)texture.w;
    float tex_top = (1.0) - (float)source_y / (float)texture.h; // not top because textures coords
                                                                // are where (0,0) is bottom left
    float tex_nw = (float)source_w / (float)texture.w;
    float tex_nh = (float)source_h / (float)texture.h;

    if (angle == 0) {
        // Draw Coords
        float left = gl_window_x(x);
        float right = gl_window_x(x + source_w * scale);
        float top = gl_window_y(y);
        float bottom = gl_window_y(y + source_h * scale);

        // If flipping, swap left and right x coords around.
        if (flip_horizontally) {
            float temp = left;
            left = right;
            right = temp;
        }

        glTexCoord2f(tex_left, tex_top);
        glVertex2f(left, top); // Top Left Corner
        glTexCoord2f(tex_left + tex_nw, tex_top);
        glVertex2f(right, top); // Top Right Corner
        glTexCoord2f(tex_left + tex_nw, tex_top - tex_nh);
        glVertex2f(right, bottom); // Bottom Right Corner
        glTexCoord2f(tex_left, tex_top - tex_nh);
        glVertex2f(left, bottom); // Bottom Left Corner
    } else {
        // pivot is relative so we have to "make it *not* relative"
        pivot_x += x;
        pivot_y += y;

        v2 top_left = { x, y };
        v2 top_right = { x + source_w, y };
        v2 bottom_right = { x + source_w, y + source_h };
        v2 bottom_left = { x, y + source_h };

        RotatePoint(&top_left, pivot_x, pivot_y, angle);
        RotatePoint(&top_right, pivot_x, pivot_y, angle);
        RotatePoint(&bottom_right, pivot_x, pivot_y, angle);
        RotatePoint(&bottom_left, pivot_x, pivot_y, angle);

        glTexCoord2f(tex_left, tex_top);
        glVertex2f(gl_window_x(top_left.x),
                   gl_window_y(top_left.y)); // Top Left Corner
        glTexCoord2f(tex_left + tex_nw, tex_top);
        glVertex2f(gl_window_x(top_right.x),
                   gl_window_y(top_right.y)); // Top Right Corner
        glTexCoord2f(tex_left + tex_nw, tex_top - tex_nh);
        glVertex2f(gl_window_x(bottom_right.x),
                   gl_window_y(bottom_right.y)); // Bottom Right Corner
        glTexCoord2f(tex_left, tex_top - tex_nh);
        glVertex2f(gl_window_x(bottom_left.x),
                   gl_window_y(bottom_left.y)); // Bottom Left Corner
    }

    glEnd();
}

void DrawTextureEx(Texture texture,
                   int x,
                   int y,
                   Rect *source,
                   int scale,
                   bool flip_horizontally,
                   v2 *pivot_point,
                   float angle) {

    int source_x, source_y, source_w, source_h;
    if (source != NULL) {
        source_x = source->x;
        source_y = source->y;
        source_w = source->w;
        source_h = source->h;
    } else {
        source_x = 0;
        source_y = 0;
        source_w = texture.w;
        source_h = texture.h;
    }

    int px, py = -1;
    if (pivot_point != NULL) {
        px = pivot_point->x;
        py = pivot_point->y;
    }
    else { // defaults to center of texture
        px = source_w / 2;
        py = source_h / 2;
    }

    DrawTextureEx(texture,
                  x,
                  y,
                  source_x,
                  source_y,
                  source_w,
                  source_h,
                  scale,
                  flip_horizontally,
                  px,
                  py,
                  angle);
}

Texture_Framebuffer CreateTextureFramebuffer(int w, int h, GLint default_buffer) {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer); // makes the buffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("the buffer is scuffed");
    }
    glBindFramebuffer(GL_FRAMEBUFFER,
                      framebuffer); // This function actually attaches it to "the
                                    // framebuffer target" aka the window for us.

    GLuint texture_for_buffer = CreateEmptyTexture(w, h);

    // GL_COLOR_ATTACHMENT0 is there because framebuffers can have multiple color
    // attachments, so by adding GL_COLOR_ATTACHMENT0 is specifying which color
    // attachment space we're taking up.
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_for_buffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        print("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, default_buffer);

    return { { texture_for_buffer, w, h }, framebuffer };
}

void ResizeTextureFramebuffer(Texture_Framebuffer *texture_framebuffer, int w, int h) {
    FreeTexture(texture_framebuffer->texture);
    texture_framebuffer->texture.gl_texture = CreateEmptyTexture(w, h);
    texture_framebuffer->texture.w = w;
    texture_framebuffer->texture.h = h;
    glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           texture_framebuffer->texture.gl_texture,
                           0);
}

void FreeTextureFramebuffer(Texture_Framebuffer *texture_framebuffer) {
    FreeTexture(texture_framebuffer->texture);
    glDeleteFramebuffers(1, &texture_framebuffer->framebuffer);
}

void SetCurrentFramebuffer(Texture_Framebuffer *texture_framebuffer) {
    // If not a texture framebuffer, then set the framebuffer to be the window.
    if (texture_framebuffer == NULL) {
        g_graphics.framebuffer_w = g_graphics.w;
        g_graphics.framebuffer_h = g_graphics.h;
        glViewport(0, 0, g_graphics.w, g_graphics.h);
        glBindFramebuffer(GL_FRAMEBUFFER, 1);
        glDrawBuffer(GL_BACK);
    } else {
        g_graphics.framebuffer_w = texture_framebuffer->texture.w;
        g_graphics.framebuffer_h = texture_framebuffer->texture.h;
        glViewport(0, 0, texture_framebuffer->texture.w, texture_framebuffer->texture.h);
        glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer->framebuffer);
    }
}

// this is yuky, redo it better
void SetCurrentFramebuffer(GLint default_buffer) {
    // If not a texture framebuffer, then set the framebuffer to be the window.
    g_graphics.framebuffer_w = g_graphics.w;
    g_graphics.framebuffer_h = g_graphics.h;
    glViewport(0, 0, g_graphics.w, g_graphics.h);
    glBindFramebuffer(GL_FRAMEBUFFER, default_buffer);
    glDrawBuffer(GL_BACK);
}

GLuint LoadShader(char *path, GLenum shader_type) {
    char *file = LoadFile(path);

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &file, NULL);
    glCompileShader(shader);

    free(file);

    // Check for errors:
    GLint success;
    GLint length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            char *log = (char *)malloc(length);
            glGetShaderInfoLog(shader, length, NULL, log);
            print("%s\n\n", log);
        }
        print("Error compiling shader %s.", path);
    }

    return shader;
}

void LinkProgram(GLuint gl_program) {
    // Apparently creates an executable, which CompileShader doesn't do?! OpenGL
    // is weird like that / I don't know enough 3D graphics.
    glLinkProgram(gl_program);

    // Checking if the program has any errors
    GLint status;
    glValidateProgram(gl_program);
    glGetProgramiv(gl_program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint len;
        glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log = bmalloc_arr(char, len);
            glGetProgramInfoLog(gl_program, len, &len, log);
            print("%s\n\n", log);
            free(log);
        }
        print("Failed to link program!");
    }
}

void PrintGLError() {
    GLenum error_code;
    while ((error_code = glGetError()) != GL_NO_ERROR) {
        switch (error_code) {
            case GL_INVALID_ENUM:
                print("%s", "INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                print("%s", "INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                print("%s", "INVALID_OPERATION");
                break;
            case GL_STACK_OVERFLOW:
                print("%s", "STACK_OVERFLOW");
                break;
            case GL_STACK_UNDERFLOW:
                print("%s", "STACK_UNDERFLOW");
                break;
            case GL_OUT_OF_MEMORY:
                print("%s", "OUT_OF_MEMORY");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                print("%s", "INVALID_FRAMEBUFFER_OPERATION");
                break;
        }
    }
}

// Light position is 0->1 relative to the texture the light is going to be used
// in. (OpenGL Texture Coordinate Format basically).
int AddLight(Light light) {
    if (g_graphics.num_lights == MAX_NUM_LIGHTS) {
        print("Cannot add any more lights!");
        return -1;
    }

    g_graphics.lights[g_graphics.num_lights] = light;

    return g_graphics.num_lights++;
}

// 99.999% of the time (not exaggerated at all), the gl_program will be
// gameplay_program. But, putting gameplay_program in g_graphics to avoid
// passing it through this function is not worth it imo.

// I know this updates lots of uniforms that aren't changing, but I'm not at the
// point where it's actually affecting performance. I could always sneak in a
// to_update boolean member. Right now it's not important, and would add
// unecessarry waffle.
void SendLightsToProgram(GLint gl_program) {
    int variable_location;

    for (int i = 0; i < g_graphics.num_lights; i++) {
        variable_location = glGetUniformLocation(
            gl_program, ("lights[" + std::to_string(i) + "].position").c_str());
        glUniform2fv(variable_location, 1, (GLfloat *)&g_graphics.lights[i].position);

        variable_location =
            glGetUniformLocation(gl_program, ("lights[" + std::to_string(i) + "].color").c_str());
        glUniform3fv(variable_location, 1, (GLfloat *)&g_graphics.lights[i].color);

        variable_location =
            glGetUniformLocation(gl_program, ("lights[" + std::to_string(i) + "].power").c_str());
        glUniform1f(variable_location, g_graphics.lights[i].power);
    }

    variable_location = glGetUniformLocation(gl_program, "num_lights");
    glUniform1i(variable_location, g_graphics.num_lights);
}

GlobalGraphicsData g_graphics = { 1400, 800, 2, NULL };

#endif