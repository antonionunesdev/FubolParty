#ifndef AUDIO_H
#define AUDIO_H

#include "game_state.h"

void initAudio();
void uninitAudio();
void playSoundEffect(const char* soundPath);
void startBowlingRollLoop();
void stopBowlingRollLoop();
void startGrassRollLoop();
void stopGrassRollLoop();
void startBochaRollLoop();
void stopBochaRollLoop();
void startMenuMusicLoop();
void stopMenuMusicLoop();

#endif // AUDIO_H
