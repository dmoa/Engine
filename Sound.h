#include <Engine/Engine.h>


Mix_Chunk* Sound_LoadWav(std::string path);
void Sound_Play(Mix_Chunk* sound);
void Sound_PlayOnLoop(Mix_Chunk* sound);
void Sound_Stop(Mix_Chunk* sound);
void Sound_Free(Mix_Chunk* sound);



#ifdef ENGINE_IMPLEMENTATION



Mix_Chunk* Sound_LoadWav(std::string path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (sound == NULL) print("Invalid path! %s", path.c_str());
    return sound;
}


void Sound_Play(Mix_Chunk* sound) {
    Mix_PlayChannel(-1, sound, 0);
}


void Sound_PlayOnLoop(Mix_Chunk* sound) {
    Mix_PlayChannel(-1, sound, -1);
}


void Sound_Stop(Mix_Chunk* sound) {
    if (!sound) return;

    for (int i = 0; i < Mix_AllocateChannels(-1); i++) {
        if (Mix_GetChunk(i) == sound && Mix_Playing(i)) {
            Mix_HaltChannel(i);
        }
    }
}


void Sound_Free(Mix_Chunk* sound) {
    Mix_FreeChunk(sound);
}


#endif