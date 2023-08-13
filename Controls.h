#pragma once

#include "utils/GetGameController.h"

// Mouse Functions

// For uniformity
#define GetMouseState SDL_GetMouseState
#define JOYSTICK_LEFTX SDL_CONTROLLER_AXIS_LEFTX
#define JOYSTICK_LEFTY SDL_CONTROLLER_AXIS_LEFTY
#define JOYSTICK_RIGHTX SDL_CONTROLLER_AXIS_RIGHTX
#define JOYSTICK_RIGHTY SDL_CONTROLLER_AXIS_RIGHTY

// minimum angle for axis to be considered "moved"
#define AXIS_MIN_MOVED 7000
// Maximum value for the axis
#define AXIS_MAX 32767

struct Map;

struct GlobalControls {
        const Uint8 *keys_down = NULL;
        SDL_GameController *controller = NULL;

        bool action_one_before = false;

        bool action_dev_before = false;
        int old_mouse_x, old_mouse_y = 0;

        void Init();

        // CALL AFTER ALL OTHER UPDATE CALLS
        void Update();

        bool Left();
        bool Right();
        bool Up();
        bool Down();

        bool Action1();
        bool Action2();
        bool Action1Event();
        bool Back();
        bool MouseMoved();
        bool ActionDev();
};
extern GlobalControls g_controls;

// Mouse

void GetMouseInGameWindowCoords(int *x, int *y);
v2i GetMouseInGameWindowCoords();
v2i GetMouseGameOverlayCoords();
bool CursorInGameWindow();

inline bool GetMouseDown(int i = 1) {
    return SDL_GetMouseState(NULL, NULL) == SDL_BUTTON(i);
};

// Joystick

// Functions with same functionality, but the names makes it slightly clearer
// when running them.
inline bool IsLeft(s16 value) {
    return value < -AXIS_MIN_MOVED;
};
inline bool IsRight(s16 value) {
    return value > AXIS_MIN_MOVED;
};
inline bool IsUp(s16 value) {
    return value < -AXIS_MIN_MOVED;
};
inline bool IsDown(s16 value) {
    return value > AXIS_MIN_MOVED;
};

inline s16 ControllerAxis(SDL_GameControllerAxis button) {
    return SDL_GameControllerGetAxis(g_controls.controller, button);
}
inline bool ControllerButton(SDL_GameControllerButton button) {
    return SDL_GameControllerGetButton(g_controls.controller, button);
}

#ifdef ENGINE_IMPLEMENTATION

void GlobalControls::Init() {
    keys_down = SDL_GetKeyboardState(NULL);
    controller = GetGameController();
}

void GlobalControls::Update() {
    action_one_before = Action1();
}

bool GlobalControls::Left() {
    return keys_down[SDL_SCANCODE_LEFT] || keys_down[SDL_SCANCODE_A] ||
           IsLeft(ControllerAxis(JOYSTICK_LEFTX));
}

bool GlobalControls::Right() {
    return keys_down[SDL_SCANCODE_RIGHT] || keys_down[SDL_SCANCODE_D] ||
           IsRight(ControllerAxis(JOYSTICK_LEFTX));
}

bool GlobalControls::Up() {
    return keys_down[SDL_SCANCODE_UP] || keys_down[SDL_SCANCODE_W] ||
           IsUp(ControllerAxis(JOYSTICK_LEFTY));
}

bool GlobalControls::Down() {
    return keys_down[SDL_SCANCODE_DOWN] || keys_down[SDL_SCANCODE_S] ||
           IsDown(ControllerAxis(JOYSTICK_LEFTY));
}

bool GlobalControls::Action1() {
    return GetMouseDown(SDL_BUTTON_LEFT) || ControllerButton(SDL_CONTROLLER_BUTTON_X);
}

bool GlobalControls::Action2() {
    return GetMouseDown(SDL_BUTTON_RIGHT) || ControllerButton(SDL_CONTROLLER_BUTTON_Y);
}

bool GlobalControls::Action1Event() {
    // If we have the mouse down and it wasn't down before, then that's an event!
    return Action1() && ! action_one_before;

}

bool GlobalControls::Back() {
    return keys_down[SDL_SCANCODE_ESCAPE] || ControllerButton(SDL_CONTROLLER_BUTTON_START);
}

bool GlobalControls::MouseMoved() {
    int new_x, new_y;
    GetMouseState(&new_x, &new_y);

    bool result = old_mouse_x != new_x || old_mouse_y != new_y;

    old_mouse_x = new_x;
    old_mouse_y = new_y;

    return result;
}

bool GlobalControls::ActionDev() {
    // To make sure the function doesn't constantly spam ActionDev is down.
    // It acts more as a key down event.
    bool action_dev_now = (keys_down[SDL_SCANCODE_LCTRL]) || (ControllerButton(SDL_CONTROLLER_BUTTON_BACK) && g_controls.Left());
    bool result = action_dev_now && !action_dev_before;
    action_dev_before = action_dev_now;

    return result;
}


GlobalControls g_controls;


v2i GetMouseInGameWindowCoords() {
    v2i return_coord = {-1, -1};
    SDL_GetMouseState(& return_coord.x, & return_coord.y);
    return_coord.x = (return_coord.x - g_graphics.gameplay_target_x) / g_graphics.scale;
    return_coord.y = (return_coord.y - g_graphics.gameplay_target_y) / g_graphics.scale;

    return return_coord;
}


v2i GetMouseGameOverlayCoords() {
    v2i return_coord = {-1, -1};
    SDL_GetMouseState(& return_coord.x, & return_coord.y);
    return_coord.x = return_coord.x / g_graphics.scale;
    return_coord.y = return_coord.y / g_graphics.scale;

    return return_coord;
}


bool CursorInGameWindow() {
    v2i coords;
    SDL_GetMouseState(& coords.x, & coords.y);
    return PointRect(coords.x, coords.y, g_graphics.gameplay_target_x, g_graphics.gameplay_target_y, g_graphics.gameplay_target_w * g_graphics.scale, g_graphics.gameplay_target_h * g_graphics.scale);
}



#endif