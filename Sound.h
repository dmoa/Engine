#include <Engine/Engine.h>


void Sound_Create(ma_sound* sound, std::string path);
void Sound_Play(std::string path);
void Sound_Play(ma_sound* sound);
void Sound_PlayOnLoop(ma_sound* sound);
void Sound_Stop(ma_sound* sound);
void Sound_Free(ma_sound* sound);




#ifdef ENGINE_IMPLEMENTATION




ma_engine sound_engine;


void Sound_Create(ma_sound* sound, std::string path) {
    ma_result result;
    // Error checking yoinked from miniaudio docs.
    result = ma_sound_init_from_file(& sound_engine, path.c_str(), 0, NULL, NULL, sound);
    if (result != MA_SUCCESS) {
        print("Error loading %s!", path.c_str());
    }
}


void Sound_Play(std::string path) {
    ma_result result = ma_engine_play_sound(& sound_engine, path.c_str(), NULL);
    if (result != MA_SUCCESS) {
        print("Error loading %s!", path.c_str());
    }
}


void Sound_Play(ma_sound* sound) {
    ma_sound_start(sound);
}


void Sound_PlayOnLoop(ma_sound* sound) {
    ma_sound_set_looping(sound, MA_TRUE);
    ma_sound_start(sound);
}


void Sound_Stop(ma_sound* sound) {
    ma_sound_stop(sound);
    ma_sound_seek_to_pcm_frame(sound, 0);
}


void Sound_Free(ma_sound* sound) {
    ma_sound_uninit(sound);
}


#endif