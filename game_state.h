#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
typedef int bool;
#define true 1
#define false 0

#define MAX_PLAYERS 4

typedef enum {
    PLAYER_SELECTION,
    SKIN_SELECTION,
    GAME_SELECTION,
    MINIGOLF,
    BOWLING,
    BOCHA,
    END_OF_GAME
} GameState;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    Vec3 pos;
    Vec3 velocity;
    bool isMoving;
    bool inHole;
    float color[4];
} Ball;

typedef enum { STANDING, FALLING, FALLEN } PinState;
typedef struct {
    Vec3 pos;
    PinState state;
    float fallAngle;
    Vec3 fallAxis;
    Vec3 velocity;
} Pin;

typedef struct {
    float* posicoes;
    float* normais;
    float* texCoords;
    float* cores;
    int numVertices;
    GLuint texturaID;
    bool temTextura;
} ModeloGLB;

extern GameState gameState;
extern int numPlayers;
extern int currentPlayer;
extern int scores[MAX_PLAYERS];

extern GameState gameSequence[3];
extern int currentGameIndex;
extern int currentRound;
extern ModeloGLB modeloPinoBoliche;
extern ModeloGLB modeloBandeiraGolfe;

extern GLuint texFundo[6];

extern GLuint bolaTextures[6];
extern int playerTextureSelections[MAX_PLAYERS];
extern const char* skinNames[6];
extern int selectingPlayer;
extern int selectedSkinIndex;

extern GLuint texGrama;
extern GLuint texParede;
extern GLuint texturaPistaBoliche;
extern GLuint texturaCanaletaBoliche;
extern GLuint texturaChaoBocha;
extern GLuint texturaObstaculoBocha;

extern Ball playerBalls[MAX_PLAYERS];
extern float aimAngle;
extern float shotPower;
extern bool isAnyBallMoving;
extern int miniGolfStrokes[MAX_PLAYERS];

extern Ball bowlingBall;
extern Pin bowlingPins[10];
extern int currentThrow;
extern bool isBallRolling;
extern bool isInGutter;
extern float bowlingAimAngle;
extern bool bowlingAimDirection;

extern Ball bochaBalls[MAX_PLAYERS];
extern float bochaAimAngle;
extern float bochaShotPower;
extern bool isAnyBochaBallMoving;
extern bool isFirstRoundComplete;
extern bool playerGetsAnotherTurn;
extern int bochaPlayersOut;
extern int bochaFinishOrder[MAX_PLAYERS];
extern bool hasPlayedFirstTurn[MAX_PLAYERS];

extern float light0_position[4];

extern bool hasPlayedPinFallSoundThisTurn;

void displayPlayerSelection();
void keyboardPlayerSelection(unsigned char key, int x, int y);
void specialKeyPlayerSelection(int key, int x, int y);
void displaySkinSelection();
void keyboardSkinSelection(unsigned char key, int x, int y);
void specialKeySkinSelection(int key, int x, int y);
void displayGameSelection();
void keyboardGameSelection(unsigned char key, int x, int y);
void displayEndOfGame();
void keyboardEndOfGame(unsigned char key, int x, int y);
void startNextGameOrRound();
void carregarTexturaComAlpha(GLuint* texturaID, const char* caminho);
void drawTexturedQuad(GLuint textureID, float x, float y, float width, float height);
void drawCenteredText(float y, const char* text);
void desenharFundo(float camX, float camY, float camZ);
void desenharModeloGLB(ModeloGLB modelo);
void desenharModeloGLBComTextura(ModeloGLB modelo, GLboolean usarTextura);
void drawText(float x, float y, char *string);
void handleWallCollisions(Ball* ball);

#endif
