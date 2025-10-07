#ifndef MINIGOLF_H
#define MINIGOLF_H

#include "game_state.h"

void initMiniGolf();
void displayMiniGolf();
void keyboardMiniGolf(unsigned char key, int x, int y);
void specialKeyMiniGolf(int key, int x, int y);
void nextTurn();
void awardMiniGolfPoints();
void drawAimIndicator();
void handleWallCollisions(Ball* ball);
bool isInsideCourse(float x, float z);

#endif // MINIGOLF_H
