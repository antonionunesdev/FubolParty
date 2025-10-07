#include "minigolf.h"
#include "utils.h"
#include "game_state.h"
#include "audio.h"

void startNextGameOrRound();

void drawScoresUI() {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char uiText[100];
    sprintf(uiText, "JOGO: GOLFE | Rodada: %d/2 | Jogador: %d | Tacadas: %d | Forca: %.1f",
            currentRound, currentPlayer + 1, miniGolfStrokes[currentPlayer], shotPower);
    glColor3f(1,1,1);
    drawText(10, 580, uiText);

    int i;
    for(i=0; i < numPlayers; i++){
        char scoreLine[50];
        sprintf(scoreLine, "Jogador %d: %d pts", i+1, scores[i]);
        drawText(650, 580 - (i*20), scoreLine);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawCourseGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texGrama);
    float ground_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ground_mat);

    glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, -0.05f, -5.0f);
        glTexCoord2f(5.0f, 0.0f); glVertex3f( 5.0f, -0.05f, -5.0f);
        glTexCoord2f(5.0f, 5.0f); glVertex3f( 5.0f, -0.05f,  5.0f);
        glTexCoord2f(0.0f, 5.0f); glVertex3f(-5.0f, -0.05f,  5.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawWallBlock(float x1, float z1, float x2, float z2) {
    float h = 0.5f;
    float y_level = -0.05f;
    float tex_width = fabs(x2 - x1);
    float tex_depth = fabs(z2 - z1);
    float tex_height = h - y_level;

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, h, z1);
        glTexCoord2f(tex_width, 0.0f); glVertex3f(x2, h, z1);
        glTexCoord2f(tex_width, tex_depth); glVertex3f(x2, h, z2);
        glTexCoord2f(0.0f, tex_depth); glVertex3f(x1, h, z2);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y_level, z1);
        glTexCoord2f(0.0f, tex_depth); glVertex3f(x1, y_level, z2);
        glTexCoord2f(tex_width, tex_depth); glVertex3f(x2, y_level, z2);
        glTexCoord2f(tex_width, 0.0f); glVertex3f(x2, y_level, z1);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y_level, z1);
        glTexCoord2f(tex_width, 0.0f); glVertex3f(x2, y_level, z1);
        glTexCoord2f(tex_width, tex_height); glVertex3f(x2, h, z1);
        glTexCoord2f(0.0f, tex_height); glVertex3f(x1, h, z1);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y_level, z2);
        glTexCoord2f(tex_width, 0.0f); glVertex3f(x1, y_level, z2);
        glTexCoord2f(tex_width, tex_height); glVertex3f(x1, h, z2);
        glTexCoord2f(0.0f, tex_height); glVertex3f(x2, h, z2);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y_level, z2);
        glTexCoord2f(tex_depth, 0.0f); glVertex3f(x1, y_level, z1);
        glTexCoord2f(tex_depth, tex_height); glVertex3f(x1, h, z1);
        glTexCoord2f(0.0f, tex_height); glVertex3f(x1, h, z2);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y_level, z1);
        glTexCoord2f(tex_depth, 0.0f); glVertex3f(x2, y_level, z2);
        glTexCoord2f(tex_depth, tex_height); glVertex3f(x2, h, z2);
        glTexCoord2f(0.0f, tex_height); glVertex3f(x2, h, z1);
    glEnd();
}

void drawCourseWalls() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texParede);
    float wall_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_mat);

    float thickness = 0.2f;

    drawWallBlock(-4.0f - thickness, -4.0f - thickness, -4.0f, 4.0f + thickness);
    drawWallBlock(4.0f, -4.0f - thickness, 4.0f + thickness, 4.0f + thickness);
    drawWallBlock(-4.0f, -4.0f - thickness, 4.0f, -4.0f);
    drawWallBlock(-2.0f, -2.0f, -2.0f + thickness, 4.0f);
    drawWallBlock(2.0f - thickness, -2.0f, 2.0f, 4.0f);
    drawWallBlock(-2.0f, -2.0f, 2.0f, -2.0f + thickness);
    drawWallBlock(-4.0f, 4.0f, -1.8f, 4.0f + thickness);
    drawWallBlock(1.8f, 4.0f, 4.0f, 4.0f + thickness);

    glDisable(GL_TEXTURE_2D);
}

void drawAimIndicator() {
    Ball* currentBall = &playerBalls[currentPlayer];
    if (isAnyBallMoving || currentBall->inHole) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);

    float angle_rad = aimAngle * M_PI / 180.0f;
    float lineLength = 0.5f + (shotPower / 10.0f) * 1.5f;

    float endX = currentBall->pos.x + sin(angle_rad) * lineLength;
    float endZ = currentBall->pos.z - cos(angle_rad) * lineLength;

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(currentBall->pos.x, currentBall->pos.y, currentBall->pos.z);
        glVertex3f(endX, currentBall->pos.y, endZ);
    glEnd();

    glDisable(GL_LINE_STIPPLE);
    glEnable(GL_LIGHTING);
}

void initMiniGolf() {
    printf("Iniciando Mini-Golfe com %d jogadores.\n", numPlayers);
    currentPlayer = 0;
    isAnyBallMoving = false;
    int i;
    for(i=0; i<numPlayers; ++i) {
        miniGolfStrokes[i] = 0;
        playerBalls[i].pos.x = -3.0f + i * 0.25f;
        playerBalls[i].pos.y = 0.1f;
        playerBalls[i].pos.z = 3.5f;
        playerBalls[i].isMoving = false;
        playerBalls[i].inHole = false;
        playerBalls[i].velocity = (Vec3){0, 0, 0};
    }
    aimAngle = 180.0f;
    shotPower = 5.0f;
}

void displayMiniGolf() {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Ball* currentBall = &playerBalls[currentPlayer];
    float camDist = 2.5f;
    float camHeight = 2.0f;
    float angle_rad = aimAngle * M_PI / 180.0f;
    float camX = currentBall->pos.x - sin(angle_rad) * camDist;
    float camZ = currentBall->pos.z + cos(angle_rad) * camDist;
    gluLookAt(camX, camHeight, camZ, currentBall->pos.x, 0.0f, currentBall->pos.z, 0.0, 1.0, 0.0);
    desenharFundo(camX, camHeight, camZ);

    drawCourseGround();
    drawCourseWalls();

    GLUquadric* quad = gluNewQuadric();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.4f);
    glDepthMask(GL_FALSE);
    float ground_plane[4] = {0.0f, 1.0f, 0.0f, 0.04f};
    float shadow_matrix[16];
    generateShadowMatrix(shadow_matrix, light0_position, ground_plane);
    int i;
    for (i = 0; i < numPlayers; i++) {
        if (!playerBalls[i].inHole) {
            glPushMatrix();
                glMultMatrixf(shadow_matrix);
                glTranslatef(playerBalls[i].pos.x, playerBalls[i].pos.y, playerBalls[i].pos.z);
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                gluSphere(quad, 0.1f, 20, 20);
            glPopMatrix();
        }
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float hole_mat[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, hole_mat);
    glPushMatrix();
    glTranslatef(3.0, -0.04f, 3.5f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.);
    glutSolidTorus(0.02, 0.2, 10, 20);
    glPopMatrix();
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.5f, 0.5f);
        desenharModeloGLBComTextura(modeloBandeiraGolfe, GL_TRUE);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    float ball_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ball_shininess = 100.0f;
    float white_mat[] = {1.0f, 1.0f, 1.0f, 1.0f};
    gluQuadricTexture(quad, GL_TRUE);
    glEnable(GL_TEXTURE_2D);

    for(i = 0; i < numPlayers; ++i) {
        if (!playerBalls[i].inHole) {
            glMaterialfv(GL_FRONT, GL_SPECULAR, ball_specular);
            glMaterialf(GL_FRONT, GL_SHININESS, ball_shininess);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white_mat);
            glBindTexture(GL_TEXTURE_2D, bolaTextures[playerTextureSelections[i]]);
            glPushMatrix();
            glTranslatef(playerBalls[i].pos.x, playerBalls[i].pos.y, playerBalls[i].pos.z);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluSphere(quad, 0.1f, 20, 20);
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);
    gluDeleteQuadric(quad);

    drawAimIndicator();
    drawScoresUI();
}

void keyboardMiniGolf(unsigned char key, int x, int y) {
    Ball* currentBall = &playerBalls[currentPlayer];
    if (key == ' ' && !isAnyBallMoving && !currentBall->inHole) {
        playSoundEffect("sons/golf_shot.wav");
        currentBall->isMoving = true;
        isAnyBallMoving = true;
        float angle_rad = aimAngle * M_PI / 180.0f;
        currentBall->velocity.x = sin(angle_rad) * shotPower * 0.03f;
        currentBall->velocity.z = -cos(angle_rad) * shotPower * 0.03f;
        miniGolfStrokes[currentPlayer]++;
    }
}

void specialKeyMiniGolf(int key, int x, int y) {
    if(isAnyBallMoving) return;
    if (key == GLUT_KEY_LEFT) aimAngle -= 3.0f;
    if (key == GLUT_KEY_RIGHT) aimAngle += 3.0f;
    if (key == GLUT_KEY_UP) shotPower += 0.5f;
    if (key == GLUT_KEY_DOWN) shotPower -= 0.5f;
    if (shotPower < 1.0f) shotPower = 1.0f;
    if (shotPower > 10.0f) shotPower = 10.0f;
}

void awardMiniGolfPoints() {
    printf("Fim da rodada de Golfe! Calculando pontos...\n");
    int i;
    for (i = 0; i < numPlayers; i++) {
        int strokes = miniGolfStrokes[i];
        int points = 0;
        if (strokes > 0) {
            if (strokes <= 10) {
                points = 11 - strokes;
            } else {
                points = 1;
            }
        }
        scores[i] += points;
        printf("Jogador %d: %d tacadas -> +%d pontos. Total: %d\n", i + 1, strokes, points, scores[i]);
    }
}

void nextTurn() {
    isAnyBallMoving = false;
    int playersFinished = 0;
    int i;
    for (i = 0; i < numPlayers; i++) {
        if (playerBalls[i].inHole) {
            playersFinished++;
        }
    }
    if (playersFinished == numPlayers) {
        awardMiniGolfPoints();
        startNextGameOrRound();
        return;
    }
    int nextPlayer = (currentPlayer + 1) % numPlayers;
    while (playerBalls[nextPlayer].inHole) {
        nextPlayer = (nextPlayer + 1) % numPlayers;
    }
    currentPlayer = nextPlayer;
    printf("Agora eh a vez do Jogador %d\n", currentPlayer + 1);
}

void handleWallCollisions(Ball* ball) {
    float r = 0.1f;
    if (ball->pos.x < -4.0f + r) { ball->pos.x = -4.0f + r; ball->velocity.x *= -1; playSoundEffect("sons/wall_hit.wav"); }
    if (ball->pos.x >  4.0f - r) { ball->pos.x =  4.0f - r; ball->velocity.x *= -1; playSoundEffect("sons/wall_hit.wav"); }
    if (ball->pos.z < -4.0f + r) { ball->pos.z = -4.0f + r; ball->velocity.z *= -1; playSoundEffect("sons/wall_hit.wav"); }

    if (ball->pos.z > 4.0f - r) {
        if (ball->pos.x >= -4.0f && ball->pos.x <= -2.0f) {
            ball->pos.z = 4.0f - r;
            ball->velocity.z *= -1;
            playSoundEffect("sons/wall_hit.wav");
        }
        else if (ball->pos.x >= 2.0f && ball->pos.x <= 4.0f) {
            ball->pos.z = 4.0f - r;
            ball->velocity.z *= -1;
            playSoundEffect("sons/wall_hit.wav");
        }
    }

    if (ball->pos.x > -2.0f - r && ball->pos.x < 2.0f + r && ball->pos.z > -2.0f - r) {
        float dist_to_left_wall = fabs(ball->pos.x - (-2.0f));
        float dist_to_right_wall = fabs(ball->pos.x - 2.0f);
        float dist_to_bottom_wall = fabs(ball->pos.z - (-2.0f));

        if (dist_to_left_wall < dist_to_right_wall && dist_to_left_wall < dist_to_bottom_wall) {
            ball->pos.x = -2.0f - r;
            ball->velocity.x *= -1;
            playSoundEffect("sons/wall_hit.wav");
        } else if (dist_to_right_wall < dist_to_left_wall && dist_to_right_wall < dist_to_bottom_wall) {
            ball->pos.x = 2.0f + r;
            ball->velocity.x *= -1;
            playSoundEffect("sons/wall_hit.wav");
        } else {
            ball->pos.z = -2.0f - r;
            ball->velocity.z *= -1;
            playSoundEffect("sons/wall_hit.wav");
        }
    }
}
