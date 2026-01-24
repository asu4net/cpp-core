#include "audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

struct Audio_State {

    ma_engine engine;
    Slot_Map<ma_sound> sounds;
    bool inited = false;

} internal g_audio;

fn audio_init() -> bool {

    if (g_audio.inited)
    {
        audio_done();
    }

    dbg_log("Audio System created!");
    auto result = ma_engine_init(nullptr, &g_audio.engine);
    if (result != MA_SUCCESS) 
    {
        dbg_check(false, "Error! Failed to initialize the audio engine!");
        return false;
    }
    g_audio.inited = true;
    return true;
}

fn audio_done() -> void {
    if (g_audio.inited) {
        dbg_log("Audio System destroyed!");
        ma_engine_uninit(&g_audio.engine);
        g_audio.inited = false;
    }
}

fn audio_load(const std::string& filename) -> Audio_Handle {\
    auto& [handle, sound] = g_audio.sounds.insert();
    auto result = ma_sound_init_from_file(&g_audio.engine, filename.c_str(), 0, nullptr, nullptr, sound);
    if (result != MA_SUCCESS)
    {
        dbg_check(false, "Error! Failed to load the sound!");
        return {};
    }
    return handle;
}

fn audio_free(const Audio_Handle& handle) -> void {
    auto* sound = g_audio.sounds.get(handle);
    if (sound)
    ma_sound_uninit(sound);
}

fn audio_play(const Audio_Handle& handle) -> void {
    auto* sound = g_audio.sounds.get(handle);
    if (sound)
    ma_sound_start(sound);
}

fn audio_stop(const Audio_Handle& handle) -> void {
    auto* sound = g_audio.sounds.get(handle);
    if (sound)
    ma_sound_stop(sound);
}

fn audio_set_volume(const Audio_Handle& handle, f32 volume) -> void {
    auto* sound = g_audio.sounds.get(handle);
    if (sound)
    ma_sound_set_volume(sound, volume);
}