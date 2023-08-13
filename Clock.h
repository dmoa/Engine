#pragma once

// The more previous fps we store, the more accurate the average fps.
#define ACCURACY 50
#define SEC_PER_TICK 1.0/60.0

extern double g_dt;
extern double g_time_elapsed;

struct EngineClock {
        void tick();

        double last_tick_time = 0;

        // for average fps
        double fpss[ACCURACY];
        int fpss_index = -1;
        int average_fps = -1;
        char average_fps_str[5];
};
// odd naming to avoid collision with clock() in <time.h>

#ifdef ENGINE_IMPLEMENTATION

void EngineClock::tick() {
    // DT

    double tick_time = SDL_GetTicks();
    g_dt = (tick_time - last_tick_time) / 1000;
    last_tick_time = tick_time;

    // Total Time

    g_time_elapsed += g_dt;

    // Average FPS

    double sum = 0;
    for (int i = 0; i < ACCURACY; i++) {
        sum += fpss[i];
    }
    average_fps = sum / ACCURACY;

    if (g_dt != 0)
        fpss[fpss_index] = 1 / g_dt;
    fpss_index = (fpss_index + 1) % ACCURACY;

    // SDL_GetTicks() returns junk values the first few times.
    // This means that average_fps may be some very long number, more than
    // the 5 characters that we allocated for it. And so, we do this range bound
    // to make sure itoa doesn't overide other memory we're using.
    if (average_fps > 0 && average_fps < 9999)
        itoa(average_fps, average_fps_str, 10);
}

double g_dt = 0.f;
double g_time_elapsed = 0.f;

#endif