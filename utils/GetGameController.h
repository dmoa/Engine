#pragma once

#include "../Engine.h"

#define FreeGameController SDL_GameControllerClose

// If there are ever multiple controllers connected and or multiple people
// playing, I'll have to change this so that it returns an array of
// SDL_GameController pointers.

inline SDL_GameController *GetGameController() {
    SDL_GameController *controller = NULL;

    // Not using right now, but will relatively soon.
    int num_controllers_connected = 0;

    // go through every joystick attatched to the computer
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        // is it supported by sdl?
        if (SDL_IsGameController(i)) {
            // then set the controller to this joystick
            controller = SDL_GameControllerOpen(i);

            // if it managed to set it, exit the function
            if (controller) {
                num_controllers_connected++;
                return controller;
            }
            // otherwise, throw error that the game controller couldn't attatch to the
            // joystick
            else {
                print("Could not open SDL gamecontroller %i: %s.", i, SDL_GetError());
            }
        }
    }

    return NULL;
}
