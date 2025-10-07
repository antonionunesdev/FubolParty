#include "bocha.h"
#include "utils.h"
#include "game_state.h"
#include "audio.h"

extern GLuint texParede;
extern GLuint texturaObstaculoBocha;
extern GLuint texturaChaoBocha;

void startNextGameOrRound();

void drawScoresUI_Bocha() {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char uiText[100];
    sprintf(uiText, "JOGO: BOCHA | Rodada: %d/2 | Jogador: %d | Forca: %.1f",
            currentRound, currentPlayer + 1, bochaShotPower);
    glColor3f(1.0, 1.0, 1.0);
    drawText(10, 580, uiText);

    int i;
    for(i=0; i < numPlayers; i++){
        char scoreLine[50];
        sprintf(scoreLine, "Jogador %d: %d pts", i+1, scores[i]);
        drawText(650, 580 - (i*20), scoreLine);
    }
    drawText(10, 20, "Setas: Mirar e Forca | Espaco: Arremessar");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawTexturedCube(float size) {
    float half = size / 2.0f;
    float repeat = 2.0f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-half, -half,  half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f( half, -half,  half);
    glTexCoord2f(repeat, repeat);      glVertex3f( half,  half,  half);
    glTexCoord2f(0.0f, repeat);        glVertex3f(-half,  half,  half);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-half, -half, -half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f( half, -half, -half);
    glTexCoord2f(repeat, repeat);      glVertex3f( half,  half, -half);
    glTexCoord2f(0.0f, repeat);        glVertex3f(-half,  half, -half);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-half,  half, -half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f( half,  half, -half);
    glTexCoord2f(repeat, repeat);      glVertex3f( half,  half,  half);
    glTexCoord2f(0.0f, repeat);        glVertex3f(-half,  half,  half);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-half, -half, -half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f( half, -half, -half);
    glTexCoord2f(repeat, repeat);      glVertex3f( half, -half,  half);
    glTexCoord2f(0.0f, repeat);        glVertex3f(-half, -half,  half);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f( half, -half, -half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f( half, -half,  half);
    glTexCoord2f(repeat, repeat);      glVertex3f( half,  half,  half);
    glTexCoord2f(0.0f, repeat);        glVertex3f( half,  half, -half);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-half, -half, -half);
    glTexCoord2f(repeat, 0.0f);        glVertex3f(-half, -half,  half);
    glTexCoord2f(repeat, repeat);      glVertex3f(-half,  half,  half);
    glTexCoord2f(0.0f, repeat);        glVertex3f(-half,  half, -half);
    glEnd();
}


void drawArenaAndObstacles() {
    float white_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular_mat[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float shininess = 10.0f;
    float arena_size = 30.0f;
    float half_size = arena_size / 2.0f;
    float wall_height = 2.0f;
    float wall_thickness = 0.5f;

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_mat);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glBindTexture(GL_TEXTURE_2D, texParede);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glBindTexture(GL_TEXTURE_2D, texturaChaoBocha);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(-half_size, 0.0f, -half_size);
        glTexCoord2f(15.0f, 0.0f);  glVertex3f( half_size, 0.0f, -half_size);
        glTexCoord2f(15.0f, 15.0f); glVertex3f( half_size, 0.0f,  half_size);
        glTexCoord2f(0.0f, 15.0f);  glVertex3f(-half_size, 0.0f,  half_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texParede);

    glPushMatrix();
    glTranslatef(0.0f, wall_height / 2.0f, -half_size);
    glScalef(arena_size + wall_thickness, wall_height, wall_thickness);
    drawTexturedCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, wall_height / 2.0f, half_size);
    glScalef(arena_size + wall_thickness, wall_height, wall_thickness);
    drawTexturedCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-half_size, wall_height / 2.0f, 0.0f);
    glScalef(wall_thickness, wall_height, arena_size - wall_thickness);
    drawTexturedCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(half_size, wall_height / 2.0f, 0.0f);
    glScalef(wall_thickness, wall_height, arena_size - wall_thickness);
    drawTexturedCube(1.0f);
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, texturaObstaculoBocha);
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);
    glMaterialfv(GL_FRONT, GL_AMBIENT, white_mat);

    float ramp_w = 0.5f; float ramp_l = 2.5f; float ramp_h = 1.0f;
    glBegin(GL_QUADS);
        glNormal3f(0.0, -1.0, 0.0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ramp_w, 0.0, -ramp_l);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( ramp_w, 0.0, -ramp_l);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( ramp_w, 0.0,  ramp_l);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-ramp_w, 0.0,  ramp_l);
        glNormal3f(0.0, 0.0, -1.0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ramp_w, 0.0, -ramp_l);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( ramp_w, 0.0, -ramp_l);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( ramp_w, ramp_h, -ramp_l);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-ramp_w, ramp_h, -ramp_l);
        glNormal3f(0.0, 0.37, 0.92);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ramp_w, 0.0, ramp_l);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( ramp_w, 0.0, ramp_l);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( ramp_w, ramp_h, -ramp_l);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-ramp_w, ramp_h, -ramp_l);
    glEnd();
    glBegin(GL_TRIANGLES);
        glNormal3f(-1.0, 0.0, 0.0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ramp_w, 0.0, -ramp_l);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-ramp_w, 0.0,  ramp_l);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(-ramp_w, ramp_h,-ramp_l);
        glNormal3f(1.0, 0.0, 0.0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( ramp_w, 0.0, -ramp_l);
        glTexCoord2f(0.5f, 1.0f); glVertex3f( ramp_w, ramp_h,-ramp_l);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( ramp_w, 0.0,  ramp_l);
    glEnd();

    glPushMatrix(); glTranslatef(4.0f, 0.5f, -5.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-4.0f, 0.5f, 5.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(8.0f, 0.5f, 8.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-8.0f, 0.5f, -8.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(8.0f, 0.5f, -8.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-8.0f, 0.5f, 8.0f); drawTexturedCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.5f, 10.0f); glScalef(5.0f, 1.0f, 1.0f); drawTexturedCube(1.0f); glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

void drawBochaAimIndicator() {
    Ball* currentBall = &bochaBalls[currentPlayer];
    if (isAnyBochaBallMoving || currentBall->inHole) return;
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(currentBall->pos.x, 0.02f, currentBall->pos.z);
    glRotatef(bochaAimAngle, 0.0f, 1.0f, 0.0f);
    float indicatorLength = 1.0f + bochaShotPower * 0.15f;
    float indicatorWidth = 0.08f;
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

void initBocha() {
    printf("Iniciando Bocha com %d jogadores.\n", numPlayers);
    currentPlayer = 0;
    isAnyBochaBallMoving = false;
    isFirstRoundComplete = false;
    playerGetsAnotherTurn = false;
    bochaPlayersOut = 0;

    int i;
    for (i = 0; i < numPlayers; ++i) {
        hasPlayedFirstTurn[i] = false;
        bochaBalls[i].pos.x = -4.5f + i * 3.0f;
        bochaBalls[i].pos.y = 0.2f;
        bochaBalls[i].pos.z = 12.0f;
        bochaBalls[i].isMoving = false;
        bochaBalls[i].inHole = false;
        bochaBalls[i].velocity = (Vec3){0, 0, 0};
    }

    bochaAimAngle = 180.0f;
    bochaShotPower = 5.0f;
}

void displayBocha() {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Ball* currentBall = &bochaBalls[currentPlayer];
    float angle_rad = bochaAimAngle * M_PI / 180.0f;
    float camHeight = 1.2f;
    float camDistBehind = 2.0f;
    float eyeX = currentBall->pos.x + sin(angle_rad) * camDistBehind;
    float eyeY = camHeight;
    float eyeZ = currentBall->pos.z + cos(angle_rad) * camDistBehind;
    float centerX = currentBall->pos.x - sin(angle_rad) * 5.0f;
    float centerY = 0.1f;
    float centerZ = currentBall->pos.z - cos(angle_rad) * 5.0f;
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0.0f, 1.0f, 0.0f);
    desenharFundo(eyeX, eyeY, eyeZ);

    drawArenaAndObstacles();

    GLUquadric* quad = gluNewQuadric();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glDepthMask(GL_FALSE);

    float ground_plane[4] = {0.0f, 1.0f, 0.0f, -0.01f};
    float shadow_matrix[16];

    int i;
    for (i = 0; i < numPlayers; i++) {
        if (!bochaBalls[i].inHole) {
            glPushMatrix();
                generateShadowMatrix(shadow_matrix, light0_position, ground_plane);
                glMultMatrixf(shadow_matrix);

                glTranslatef(bochaBalls[i].pos.x, bochaBalls[i].pos.y, bochaBalls[i].pos.z);
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                gluSphere(quad, 0.2f, 20, 20);
            glPopMatrix();
        }
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float ball_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ball_shininess = 100.0f;
    float white_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};

    gluQuadricTexture(quad, GL_TRUE);
    glEnable(GL_TEXTURE_2D);

    for (i = 0; i < numPlayers; ++i) {
        if (!bochaBalls[i].inHole) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);
            glMaterialfv(GL_FRONT, GL_SPECULAR, ball_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, ball_shininess);
            glBindTexture(GL_TEXTURE_2D, bolaTextures[playerTextureSelections[i]]);
            glPushMatrix();
                glTranslatef(bochaBalls[i].pos.x, bochaBalls[i].pos.y, bochaBalls[i].pos.z);
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                gluSphere(quad, 0.2f, 20, 20);
            glPopMatrix();
        }
    }

    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quad);

    drawBochaAimIndicator();
    drawScoresUI_Bocha();
}

void keyboardBocha(unsigned char key, int x, int y) {
    Ball* currentBall = &bochaBalls[currentPlayer];
    if (key == ' ' && !isAnyBochaBallMoving && !currentBall->inHole) {
        currentBall->isMoving = true;
        isAnyBochaBallMoving = true;
        hasPlayedFirstTurn[currentPlayer] = true;
        startBochaRollLoop();
        float angle_rad = bochaAimAngle * M_PI / 180.0f;
        currentBall->velocity.x = -(sin(angle_rad) * bochaShotPower * 0.04f);
        currentBall->velocity.z = -(cos(angle_rad) * bochaShotPower * 0.04f);
    }
}

void specialKeyBocha(int key, int x, int y) {
    if (isAnyBochaBallMoving) return;
    if (key == GLUT_KEY_RIGHT) bochaAimAngle -= 3.0f;
    if (key == GLUT_KEY_LEFT) bochaAimAngle += 3.0f;
    if (key == GLUT_KEY_UP) bochaShotPower += 0.5f;
    if (key == GLUT_KEY_DOWN) bochaShotPower -= 0.5f;
    if (bochaShotPower < 1.0f) bochaShotPower = 1.0f;
    if (bochaShotPower > 10.0f) bochaShotPower = 10.0f;
}

void awardBochaPoints() {
    printf("Fim da rodada de Bocha! Calculando pontos...\n");
    int points[] = {10, 7, 4, 1};
    int i;
    for(i=0; i < numPlayers; i++) {
        if(!bochaBalls[i].inHole) {
             bool alreadyCounted = false;
            int j;
            for(j = 0; j < bochaPlayersOut; j++) {
                if(bochaFinishOrder[j] == i) {
                    alreadyCounted = true;
                    break;
                }
            }
            if (!alreadyCounted) {
                bochaFinishOrder[bochaPlayersOut++] = i;
            }
        }
    }
    for (i = 0; i < bochaPlayersOut; i++) {
        int playerIndex = bochaFinishOrder[bochaPlayersOut - 1 - i];
        int awardedPoints = (i < 4) ? points[i] : 0;
        scores[playerIndex] += awardedPoints;
        printf("Posicao %d: Jogador %d -> +%d pontos. Total: %d\n", i + 1, playerIndex + 1, awardedPoints, scores[playerIndex]);
    }
}

void nextBochaTurn() {
    stopBochaRollLoop();
    isAnyBochaBallMoving = false;

    if (!isFirstRoundComplete) {
        bool allHavePlayed = true;
        int i;
        for (i = 0; i < numPlayers; i++) {
            if (!bochaBalls[i].inHole && !hasPlayedFirstTurn[i]) {
                allHavePlayed = false;
                break;
            }
        }
        if (allHavePlayed) {
            isFirstRoundComplete = true;
            printf("\n*** IMUNIDADE DA PRIMEIRA RODADA ACABOU! ***\n\n");
        }
    }

    int i;
    for(i = 0; i < numPlayers; i++) {
        if(bochaBalls[i].inHole) {
            bool alreadyOut = false;
            int j;
            for(j = 0; j < bochaPlayersOut; j++) {
                if(bochaFinishOrder[j] == i) {
                    alreadyOut = true;
                    break;
                }
            }
            if(!alreadyOut) {
                bochaFinishOrder[bochaPlayersOut++] = i;
            }
        }
    }

    int playersInGame = numPlayers - bochaPlayersOut;
    if (playersInGame <= 1) {
        awardBochaPoints();
        startNextGameOrRound();
        return;
    }

    if (playerGetsAnotherTurn) {
        playerGetsAnotherTurn = false;
        printf("Acertou! Jogador %d joga novamente.\n", currentPlayer + 1);
        return;
    }

    int nextPlayer = (currentPlayer + 1) % numPlayers;
    while (bochaBalls[nextPlayer].inHole) {
        nextPlayer = (nextPlayer + 1) % numPlayers;
    }

    currentPlayer = nextPlayer;
    printf("Agora e a vez do Jogador %d\n", currentPlayer + 1);
}
