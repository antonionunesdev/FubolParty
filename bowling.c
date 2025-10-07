#include "bowling.h"
#include "utils.h"
#include "game_state.h"
#include "audio.h"

extern void startNextGameOrRound();
void drawBowlingScene(float camX, float camY, float camZ);

void drawScoresUI_Bowling() {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char uiText[120];
    sprintf(uiText, "JOGO: BOLICHE | Rodada: %d/2 | Jogador: %d | Arremesso: %d | Forca: %.1f",
        currentRound, currentPlayer + 1, currentThrow, shotPower);
    glColor3f(1.0, 1.0, 1.0);
    drawText(10, 580, uiText);

    int i;
    for(i=0; i < numPlayers; i++){
        char scoreLine[50];
        sprintf(scoreLine, "Jogador %d: %d pts", i+1, scores[i]);
        drawText(650, 580 - (i*20), scoreLine);
    }

    drawText(10, 20, "Cima/Baixo: Forca | Espaco: Arremessar");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawPin() {
    glPushMatrix();
        glRotatef(-90, 1.0, 0.0, 0.0);
        desenharModeloGLBComTextura(modeloPinoBoliche, GL_TRUE);
    glPopMatrix();
}

void drawBowlingAimIndicator() {
    if (isBallRolling) return;
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(bowlingBall.pos.x, 0.01f, bowlingBall.pos.z);
    glRotatef(bowlingAimAngle, 0.0f, 1.0f, 0.0f);
    float indicatorLength = 1.5f + shotPower * 0.2f;
    float indicatorWidth = 0.1f;
    float arrowheadHeight = 0.3f;
    float arrowheadWidth = 0.25f;
    glBegin(GL_QUADS);
        glVertex3f(-indicatorWidth / 2, 0.0f, 0.0f);
        glVertex3f( indicatorWidth / 2, 0.0f, 0.0f);
        glVertex3f( indicatorWidth / 2, 0.0f, -indicatorLength);
        glVertex3f(-indicatorWidth / 2, 0.0f, -indicatorLength);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, -indicatorLength - arrowheadHeight);
        glVertex3f(-arrowheadWidth / 2, 0.0f, -indicatorLength);
        glVertex3f( arrowheadWidth / 2, 0.0f, -indicatorLength);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void resetPinsAndBall() {
    bowlingBall.isMoving = false;
    isBallRolling = false;
    isInGutter = false;
    bowlingBall.pos.x = 0.0f;
    bowlingBall.pos.y = 0.4f;
    bowlingBall.pos.z = 16.0f;
    bowlingBall.velocity = (Vec3){0, 0, 0};
    shotPower = 5.0f;
    bowlingAimAngle = 0.0f;
    bowlingAimDirection = true;

    float pin_spacing = 0.8f;
    float row_spacing = 0.7f;
    int pin_index = 0;
    int row;
    for(row = 0; row < 4; ++row) {
        int pins_in_row = row + 1;
        float start_x = - (float)row * pin_spacing / 2.0f;
        int p;
        for (p = 0; p < pins_in_row; ++p) {
            bowlingPins[pin_index].pos.x = start_x + p * pin_spacing;
            bowlingPins[pin_index].pos.y = 0.0f;
            bowlingPins[pin_index].pos.z = -15.0f - row * row_spacing;
            bowlingPins[pin_index].state = STANDING;
            bowlingPins[pin_index].fallAngle = 0.0f;
            bowlingPins[pin_index].velocity = (Vec3){0, 0, 0};
            pin_index++;
        }
    }
}

void initBowling() {
    printf("Iniciando Boliche com %d jogadores.\n", numPlayers);
    currentPlayer = 0;
    currentThrow = 1;
    resetPinsAndBall();
}

void displayBowling() {
    int screenW = glutGet(GLUT_WINDOW_WIDTH);
    int screenH = glutGet(GLUT_WINDOW_HEIGHT);

    glViewport(0, 0, screenW, screenH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)screenW / (double)screenH, 1.0, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float camX = 0.0f, camY = 4.0f, camZ = 21.0f;
    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    drawBowlingScene(camX, camY, camZ);
    drawBowlingAimIndicator();

    int vp_width = screenW / 3.5;
    int vp_height = screenH / 3.5;
    int vp_x = screenW - vp_width - 10;
    int vp_y = 10;

    glViewport(vp_x, vp_y, vp_width, vp_height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(vp_x, vp_y, vp_width, vp_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, (double)vp_width / (double)vp_height, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float followCamX = bowlingBall.pos.x;
    float followCamY = bowlingBall.pos.y + 1.5f;
    float followCamZ = bowlingBall.pos.z + 2.5f;
    float lookAtX = bowlingBall.pos.x;
    float lookAtY = bowlingBall.pos.y;
    float lookAtZ = bowlingBall.pos.z - 10.0f;
    gluLookAt(followCamX, followCamY, followCamZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);

    drawBowlingScene(followCamX, followCamY, followCamZ);
    glViewport(0, 0, screenW, screenH);
    drawScoresUI_Bowling();
}

void drawBowlingScene(float camX, float camY, float camZ) {
    desenharFundo(camX, camY, camZ);

    glEnable(GL_TEXTURE_2D);
    float white_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);

    glBindTexture(GL_TEXTURE_2D, texturaPistaBoliche);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(-2.0f, 0.0f, 17.5f);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f( 2.0f, 0.0f, 17.5f);
        glTexCoord2f(1.0f, 10.0f); glVertex3f( 2.0f, 0.0f, -17.5f);
        glTexCoord2f(0.0f, 10.0f); glVertex3f(-2.0f, 0.0f, -17.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texturaCanaletaBoliche);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(-3.0f, -0.1f, 17.5f);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(-2.0f, -0.1f, 17.5f);
        glTexCoord2f(1.0f, 10.0f); glVertex3f(-2.0f, -0.1f, -17.5f);
        glTexCoord2f(0.0f, 10.0f); glVertex3f(-3.0f, -0.1f, -17.5f);
    glEnd();
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(2.0f, -0.1f, 17.5f);
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(3.0f, -0.1f, 17.5f);
        glTexCoord2f(1.0f, 10.0f); glVertex3f(3.0f, -0.1f, -17.5f);
        glTexCoord2f(0.0f, 10.0f); glVertex3f(2.0f, -0.1f, -17.5f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glDepthMask(GL_FALSE);

    float ground_plane[4] = {0.0f, 1.0f, 0.0f, -0.01f};
    float shadow_matrix[16];
    generateShadowMatrix(shadow_matrix, light0_position, ground_plane);

    int i;
    for(i=0; i<10; ++i) {
        if(bowlingPins[i].state != FALLEN) {
            glPushMatrix();
                glMultMatrixf(shadow_matrix);
                glTranslatef(bowlingPins[i].pos.x, bowlingPins[i].pos.y, bowlingPins[i].pos.z);
                if(bowlingPins[i].state == FALLING) {
                    glRotatef(bowlingPins[i].fallAngle, bowlingPins[i].fallAxis.x, 0.0f, bowlingPins[i].fallAxis.z);
                }
                drawPin();
            glPopMatrix();
        }
    }

    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();
        glMultMatrixf(shadow_matrix);
        glTranslatef(bowlingBall.pos.x, bowlingBall.pos.y, bowlingBall.pos.z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluSphere(quad, 0.4, 30, 30);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    for(i=0; i<10; ++i) {
        if(bowlingPins[i].state != FALLEN) {
            glPushMatrix();
            glTranslatef(bowlingPins[i].pos.x, bowlingPins[i].pos.y, bowlingPins[i].pos.z);
            if(bowlingPins[i].state == FALLING) {
                glRotatef(bowlingPins[i].fallAngle, bowlingPins[i].fallAxis.x, 0.0f, bowlingPins[i].fallAxis.z);
            }
            drawPin();
            glPopMatrix();
        }
    }

    gluQuadricTexture(quad, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);
    glBindTexture(GL_TEXTURE_2D, bolaTextures[playerTextureSelections[currentPlayer]]);

    glPushMatrix();
    glTranslatef(bowlingBall.pos.x, bowlingBall.pos.y, bowlingBall.pos.z);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluSphere(quad, 0.4, 30, 30);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quad);
}

void keyboardBowling(unsigned char key, int x, int y) {
    if (key == ' ' && !isBallRolling) {
        isBallRolling = true;
        bowlingBall.isMoving = true;
        float angle_rad = bowlingAimAngle * M_PI / 180.0f;
        float total_speed = shotPower * 0.05f;
        bowlingBall.velocity.x = -sin(angle_rad) * total_speed;
        bowlingBall.velocity.z = -cos(angle_rad) * total_speed;
        hasPlayedPinFallSoundThisTurn = false;
        startBowlingRollLoop();
    }
}

void specialKeyBowling(int key, int x, int y) {
    if (isBallRolling) return;
    if (key == GLUT_KEY_UP) shotPower += 0.5f;
    if (key == GLUT_KEY_DOWN) shotPower -= 0.5f;
    if (shotPower < 1.0f) shotPower = 1.0f;
    if (shotPower > 10.0f) shotPower = 10.0f;
}

void nextBowlingTurn() {
    stopBowlingRollLoop();
    int pinsDownThisTurn = 0;
    int i;
    for(i=0; i<10; i++) {
        if(bowlingPins[i].state != STANDING) {
            pinsDownThisTurn++;
        }
    }

    if (currentThrow == 2 || pinsDownThisTurn == 10) {
        scores[currentPlayer] += pinsDownThisTurn;
        printf("Jogador %d marcou %d pontos. Total acumulado: %d\n", currentPlayer + 1, pinsDownThisTurn, scores[currentPlayer]);

        currentPlayer++;
        currentThrow = 1;

        if (currentPlayer >= numPlayers) {
            printf("Fim da rodada de Boliche!\n");
            startNextGameOrRound();
            return;
        } else {
            resetPinsAndBall();
            printf("Proximo: Jogador %d\n", currentPlayer + 1);
        }

    } else {
        currentThrow++;
        bowlingBall.isMoving = false;
        isBallRolling = false;
        isInGutter = false;
        bowlingBall.pos.x = 0.0f;
        bowlingBall.pos.y = 0.4f;
        bowlingBall.pos.z = 16.0f;
        bowlingBall.velocity = (Vec3){0,0,0};
    }
}
