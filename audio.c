#define MA_IMPLEMENTATION
#include "miniaudio.h"

#include "audio.h"
#include <stdio.h>

static ma_engine engine;
static ma_sound bowling_roll_sound;
static ma_sound grass_roll_sound;
static ma_sound bocha_roll_sound;
static ma_sound menu_music_sound;
static bool is_audio_initialized = false;
static bool is_bowling_sound_initialized = false;
static bool is_grass_sound_initialized = false;
static bool is_bocha_sound_initialized = false;
static bool is_menu_music_initialized = false;

void initAudio() {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Falha ao inicializar o motor de audio.\n");
        is_audio_initialized = false;
        return;
    }

    if (ma_sound_init_from_file(&engine, "sons/ball_roll.wav", MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &bowling_roll_sound) == MA_SUCCESS) {
        ma_sound_set_looping(&bowling_roll_sound, MA_TRUE);
        is_bowling_sound_initialized = true;
    } else {
        printf("Aviso: Nao foi possivel carregar 'sons/ball_roll.wav'.\n");
    }

    if (ma_sound_init_from_file(&engine, "sons/grass_roll.wav", MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &grass_roll_sound) == MA_SUCCESS) {
        ma_sound_set_looping(&grass_roll_sound, MA_TRUE);
        is_grass_sound_initialized = true;
    } else {
        printf("Aviso: Nao foi possivel carregar 'sons/grass_roll.wav'.\n");
    }

    if (ma_sound_init_from_file(&engine, "sons/bocha_roll.wav", MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &bocha_roll_sound) == MA_SUCCESS) {
        ma_sound_set_looping(&bocha_roll_sound, MA_TRUE);
        is_bocha_sound_initialized = true;
    } else {
        printf("Aviso: Nao foi possivel carregar 'sons/bocha_roll.wav'.\n");
    }

    if (ma_sound_init_from_file(&engine, "sons/menu_music.mp3", MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &menu_music_sound) == MA_SUCCESS) {
        ma_sound_set_looping(&menu_music_sound, MA_TRUE);
        is_menu_music_initialized = true;
    } else {
        printf("Aviso: Nao foi possivel carregar 'sons/menu_music.mp3'.\n");
    }

    is_audio_initialized = true;
    printf("Sistema de audio inicializado.\n");
}

void uninitAudio() {
    if (!is_audio_initialized) return;
    if (is_bowling_sound_initialized) ma_sound_uninit(&bowling_roll_sound);
    if (is_grass_sound_initialized) ma_sound_uninit(&grass_roll_sound);
    if (is_bocha_sound_initialized) ma_sound_uninit(&bocha_roll_sound);
    if (is_menu_music_initialized) ma_sound_uninit(&menu_music_sound);
    ma_engine_uninit(&engine);
    printf("Sistema de audio finalizado.\n");
}

void playSoundEffect(const char* soundPath) {
    if (!is_audio_initialized) return;
    ma_engine_play_sound(&engine, soundPath, NULL);
}

void startBowlingRollLoop() { if (is_audio_initialized && is_bowling_sound_initialized && !ma_sound_is_playing(&bowling_roll_sound)) ma_sound_start(&bowling_roll_sound); }
void stopBowlingRollLoop() { if (is_audio_initialized && is_bowling_sound_initialized && ma_sound_is_playing(&bowling_roll_sound)) ma_sound_stop(&bowling_roll_sound); }
void startGrassRollLoop() { if (is_audio_initialized && is_grass_sound_initialized && !ma_sound_is_playing(&grass_roll_sound)) ma_sound_start(&grass_roll_sound); }
void stopGrassRollLoop() { if (is_audio_initialized && is_grass_sound_initialized && ma_sound_is_playing(&grass_roll_sound)) ma_sound_stop(&grass_roll_sound); }
void startBochaRollLoop() { if (is_audio_initialized && is_bocha_sound_initialized && !ma_sound_is_playing(&bocha_roll_sound)) ma_sound_start(&bocha_roll_sound); }
void stopBochaRollLoop() { if (is_audio_initialized && is_bocha_sound_initialized && ma_sound_is_playing(&bocha_roll_sound)) ma_sound_stop(&bocha_roll_sound); }
void startMenuMusicLoop() { if (is_audio_initialized && is_menu_music_initialized && !ma_sound_is_playing(&menu_music_sound)) ma_sound_start(&menu_music_sound); }
void stopMenuMusicLoop() { if (is_audio_initialized && is_menu_music_initialized && ma_sound_is_playing(&menu_music_sound)) ma_sound_stop(&menu_music_sound); }
