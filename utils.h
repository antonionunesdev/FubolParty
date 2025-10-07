#ifndef UTILS_H
#define UTILS_H

#include "game_state.h"

void drawText(float x, float y, char *string);
void generateShadowMatrix(float matrix[16], const float light_pos[4], const float plane[4]);

#endif // UTILS_H
