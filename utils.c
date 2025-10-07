#include "utils.h"

void drawText(float x, float y, char *string) {
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    int i;
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}

void generateShadowMatrix(float matrix[16], const float light_pos[4], const float plane[4]) {
    float dot = plane[0] * light_pos[0] + plane[1] * light_pos[1] + plane[2] * light_pos[2] + plane[3] * light_pos[3];

    matrix[0]  = dot - light_pos[0] * plane[0];
    matrix[1]  = 0.f - light_pos[1] * plane[0];
    matrix[2]  = 0.f - light_pos[2] * plane[0];
    matrix[3]  = 0.f - light_pos[3] * plane[0];

    matrix[4]  = 0.f - light_pos[0] * plane[1];
    matrix[5]  = dot - light_pos[1] * plane[1];
    matrix[6]  = 0.f - light_pos[2] * plane[1];
    matrix[7]  = 0.f - light_pos[3] * plane[1];

    matrix[8]  = 0.f - light_pos[0] * plane[2];
    matrix[9]  = 0.f - light_pos[1] * plane[2];
    matrix[10] = dot - light_pos[2] * plane[2];
    matrix[11] = 0.f - light_pos[3] * plane[2];

    matrix[12] = 0.f - light_pos[0] * plane[3];
    matrix[13] = 0.f - light_pos[1] * plane[3];
    matrix[14] = 0.f - light_pos[2] * plane[3];
    matrix[15] = dot - light_pos[3] * plane[3];
}
