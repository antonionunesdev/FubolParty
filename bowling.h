#ifndef BOWLING_H
#define BOWLING_H

#include "game_state.h"

void initBowling();
void displayBowling();
void keyboardBowling(unsigned char key, int x, int y);
void specialKeyBowling(int key, int x, int y);
void nextBowlingTurn();

#endif // BOWLING_H
