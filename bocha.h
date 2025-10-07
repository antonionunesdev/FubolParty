#ifndef BOCHA_H
#define BOCHA_H

#include "game_state.h"

void initBocha();
void displayBocha();
void keyboardBocha(unsigned char key, int x, int y);
void specialKeyBocha(int key, int x, int y);


void nextBochaTurn();

#endif // BOCHA_H
