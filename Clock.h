#pragma once

// The more previous fps we store, the more accurate the average fps.
#define ACCURACY 50

extern float g_dt;
extern float g_time_elapsed;

struct EngineClock {
        void tick();

        float last_tick_time = 0;

        // for average fps
        float fpss[ACCURACY];
        int fpss_index = -1;
        int average_fps = -1;
        char average_fps_str[5];
};
// odd naming to avoid collision with clock() in <time.h>

#ifdef ENGINE_IMPLEMENTATION

void EngineClock::tick() {
    // DT

    float tick_time = SDL_GetTicks();
    g_dt = (tick_time - last_tick_time) / 1000;
    last_tick_time = tick_time;

    // Total Time

    g_time_elapsed += g_dt;

    // Average FPS

    float sum = 0;
    for (int i = 0; i < ACCURACY; i++) {
        sum += fpss[i];
    }
    average_fps = sum / ACCURACY;

    if (g_dt != 0)
        fpss[fpss_index] = 1 / g_dt;
    fpss_index = (fpss_index + 1) % ACCURACY;
    itoa(average_fps, average_fps_str, 10);
}

float g_dt = 0.f;
float g_time_elapsed = 0.f;

#endif