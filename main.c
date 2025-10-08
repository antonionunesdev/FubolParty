#include <GL/freeglut.h>
#include "game_state.h"
#include "utils.h"
#include "minigolf.h"
#include "bowling.h"
#include "bocha.h"
#include "audio.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

GameState gameState = PLAYER_SELECTION;
int numPlayers = 2;
int currentPlayer = 0;
int scores[MAX_PLAYERS] = {0, 0, 0, 0};

GameState gameSequence[3];
int currentGameIndex = 0;
int currentRound = 1;

GLuint texFundo[6];
GLuint logoTextureID;
GLuint bolaTextures[6];

const char* skyboxFiles[6] = {
    "texturas/corona_ft.png",
    "texturas/corona_bk.png",
    "texturas/corona_lf.png",
    "texturas/corona_rt.png",
    "texturas/corona_up.png",
    "texturas/corona_dn.png"
};

const char* bolaTextureFiles[6] = {
    "texturas/planeta.jpg",
    "texturas/ouro.jpg",
    "texturas/bola_vermelha.jpg",
    "texturas/bola_azul.jpg",
    "texturas/bola_verde.jpg",
    "texturas/estrada.jpg"
};

const char* skinNames[6] = {
    "Planeta",
    "Ouro",
    "Vermelha",
    "Azul",
    "Verde",
    "Estrada"
};

int playerTextureSelections[MAX_PLAYERS] = {-1, -1, -1, -1};
int selectingPlayer = 0;
int selectedSkinIndex = 0;

ModeloGLB modeloPinoBoliche;
ModeloGLB modeloBandeiraGolfe;

bool hasPlayedFirstTurn[MAX_PLAYERS] = {false, false, false, false};

float light0_position[] = { 5.0f, 10.0f, 5.0f, 1.0f };

Ball playerBalls[MAX_PLAYERS];
float aimAngle = 0.0f;
float shotPower = 5.0f;
bool isAnyBallMoving = false;
int miniGolfStrokes[MAX_PLAYERS] = {0,0,0,0};
GLuint texGrama;
GLuint texParede;

Ball bowlingBall;
Pin bowlingPins[10];
int currentThrow = 1;
bool isBallRolling = false;
bool isInGutter = false;
float bowlingAimAngle = 0.0f;
bool bowlingAimDirection = true;
GLuint texturaPistaBoliche;
GLuint texturaCanaletaBoliche;

Ball bochaBalls[MAX_PLAYERS];
float bochaAimAngle = 0.0f;
float bochaShotPower = 5.0f;
bool isAnyBochaBallMoving = false;
bool isFirstRoundComplete = false;
bool playerGetsAnotherTurn = false;
int bochaPlayersOut = 0;
int bochaFinishOrder[MAX_PLAYERS];
GLuint texturaChaoBocha;
GLuint texturaObstaculoBocha;
bool hasPlayedPinFallSoundThisTurn = false;

ModeloGLB carregarModeloGLB(const char* caminhoArquivo) {
    ModeloGLB modelo = {NULL, NULL, NULL, NULL, 0, 0, false};
    cgltf_options options = {0};
    cgltf_data* data = NULL;

    if (cgltf_parse_file(&options, caminhoArquivo, &data) != cgltf_result_success ||
        cgltf_load_buffers(&options, data, caminhoArquivo) != cgltf_result_success) {
        printf("Erro ao carregar ou processar o modelo: %s\n", caminhoArquivo);
        cgltf_free(data);
        return modelo;
    }

    if (data->meshes_count == 0 || data->meshes[0].primitives_count == 0) {
        printf("Nenhuma mesh/primitiva encontrada no arquivo: %s\n", caminhoArquivo);
        cgltf_free(data);
        return modelo;
    }
    cgltf_primitive* primitive = &data->meshes[0].primitives[0];

    if (primitive->material &&
        primitive->material->has_pbr_metallic_roughness &&
        primitive->material->pbr_metallic_roughness.base_color_texture.texture) {

        cgltf_texture* texture = primitive->material->pbr_metallic_roughness.base_color_texture.texture;
        cgltf_image* image = texture->image;
        cgltf_buffer_view* buffer_view = image->buffer_view;

        const unsigned char* imageData = (const unsigned char*)buffer_view->buffer->data + buffer_view->offset;
        cgltf_size imageSize = buffer_view->size;

        int largura, altura, canais;
        unsigned char *dadosTextura = stbi_load_from_memory(imageData, imageSize, &largura, &altura, &canais, 0);

        if (dadosTextura) {
            glGenTextures(1, &modelo.texturaID);
            glBindTexture(GL_TEXTURE_2D, modelo.texturaID);

            GLenum format = (canais == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, largura, altura, 0, format, GL_UNSIGNED_BYTE, dadosTextura);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(dadosTextura);
            modelo.temTextura = true;
            printf("Textura embutida carregada para o modelo '%s' (ID=%d)\n", caminhoArquivo, modelo.texturaID);
        } else {
            printf("Erro ao decodificar a textura embutida para: %s\n", caminhoArquivo);
        }
    }

    if (primitive->indices != NULL) {
        cgltf_accessor* indices = primitive->indices;
        modelo.numVertices = indices->count;
        modelo.posicoes = (float*)malloc(modelo.numVertices * 3 * sizeof(float));
        modelo.normais = (float*)malloc(modelo.numVertices * 3 * sizeof(float));
        modelo.texCoords = (float*)malloc(modelo.numVertices * 2 * sizeof(float));
        float* temp_positions = NULL, *temp_normals = NULL, *temp_texcoords = NULL;
        size_t pos_count = 0, norm_count = 0, tex_count = 0;
        for (cgltf_size i = 0; i < primitive->attributes_count; ++i) {
            cgltf_attribute* attr = &primitive->attributes[i];
            if (attr->type == cgltf_attribute_type_position) {
                pos_count = attr->data->count;
                temp_positions = (float*)malloc(pos_count * 3 * sizeof(float));
                cgltf_accessor_unpack_floats(attr->data, temp_positions, pos_count * 3);
            } else if (attr->type == cgltf_attribute_type_normal) {
                norm_count = attr->data->count;
                temp_normals = (float*)malloc(norm_count * 3 * sizeof(float));
                cgltf_accessor_unpack_floats(attr->data, temp_normals, norm_count * 3);
            } else if (attr->type == cgltf_attribute_type_texcoord) {
                tex_count = attr->data->count;
                temp_texcoords = (float*)malloc(tex_count * 2 * sizeof(float));
                cgltf_accessor_unpack_floats(attr->data, temp_texcoords, tex_count * 2);
            }
        }
        for (size_t i = 0; i < modelo.numVertices; ++i) {
            cgltf_uint index = cgltf_accessor_read_index(indices, i);
            if (temp_positions && index < pos_count) memcpy(&modelo.posicoes[i * 3], &temp_positions[index * 3], 3 * sizeof(float));
            if (temp_normals && index < norm_count) memcpy(&modelo.normais[i * 3], &temp_normals[index * 3], 3 * sizeof(float));
            if (temp_texcoords && index < tex_count) memcpy(&modelo.texCoords[i * 2], &temp_texcoords[index * 2], 2 * sizeof(float));
        }
        free(temp_positions); free(temp_normals); free(temp_texcoords);
    } else {
        modelo.numVertices = primitive->attributes[0].data->count;
        modelo.posicoes = (float*)malloc(modelo.numVertices * 3 * sizeof(float));
        modelo.normais = (float*)malloc(modelo.numVertices * 3 * sizeof(float));
        modelo.texCoords = (float*)malloc(modelo.numVertices * 2 * sizeof(float));
        for (cgltf_size i = 0; i < primitive->attributes_count; ++i) {
            cgltf_attribute* attr = &primitive->attributes[i];
            if (attr->type == cgltf_attribute_type_position) cgltf_accessor_unpack_floats(attr->data, modelo.posicoes, modelo.numVertices * 3);
            else if (attr->type == cgltf_attribute_type_normal) cgltf_accessor_unpack_floats(attr->data, modelo.normais, modelo.numVertices * 3);
            else if (attr->type == cgltf_attribute_type_texcoord) cgltf_accessor_unpack_floats(attr->data, modelo.texCoords, modelo.numVertices * 2);
        }
    }

    cgltf_free(data);
    printf("Modelo '%s' carregado: %d vertices\n", caminhoArquivo, modelo.numVertices);
    return modelo;
}

void desenharModeloGLBComTextura(ModeloGLB modelo, GLboolean usarTexturaExterna) {
    if (!modelo.posicoes) return;
    bool usarTexturaDoModelo = modelo.temTextura && modelo.texCoords;
    if (usarTexturaDoModelo) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, modelo.texturaID);
        float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        float spec[] = {0.2f, 0.2f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
        glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
        glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
    }

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < modelo.numVertices; ++i) {
        if (modelo.normais) glNormal3fv(&modelo.normais[i * 3]);
        if (usarTexturaDoModelo) glTexCoord2fv(&modelo.texCoords[i * 2]);
        glVertex3fv(&modelo.posicoes[i * 3]);
    }
    glEnd();

    if (usarTexturaDoModelo) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void desenharModeloGLB(ModeloGLB modelo) {
    desenharModeloGLBComTextura(modelo, GL_FALSE);
}

void carregarTextura(GLuint* texturaID, const char* caminho) {
    int largura, altura, canais;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *dados = stbi_load(caminho, &largura, &altura, &canais, 3);
    if (dados == NULL) {
        printf("Erro ao carregar a textura: %s\n", caminho);
        return;
    }
    glGenTextures(1, texturaID);
    glBindTexture(GL_TEXTURE_2D, *texturaID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(dados);
    printf("Textura %s carregada com ID %d\n", caminho, *texturaID);
}

void carregarTexturaComAlpha(GLuint* texturaID, const char* caminho) {
    int largura, altura, canais;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *dados = stbi_load(caminho, &largura, &altura, &canais, 0);
    stbi_set_flip_vertically_on_load(false);

    if (dados == NULL) {
        printf("Erro ao carregar a textura com alpha: %s\n", caminho);
        return;
    }

    glGenTextures(1, texturaID);
    glBindTexture(GL_TEXTURE_2D, *texturaID);
    GLenum format = (canais == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, largura, altura, 0, format, GL_UNSIGNED_BYTE, dados);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(dados);
    printf("Textura com alpha '%s' carregada com ID %d\n", caminho, *texturaID);
}


void carregarTexturasDoJogo() {
    carregarTexturaComAlpha(&logoTextureID, "texturas/logo.png");
    carregarTextura(&texGrama, "texturas/Chão.png");
    carregarTextura(&texParede, "texturas/Parede.png");
    carregarTextura(&texturaPistaBoliche, "texturas/chao2boliche.jpg");
    carregarTextura(&texturaCanaletaBoliche, "texturas/chao1boliche.jpg");
    carregarTextura(&texturaChaoBocha, "texturas/chãoBocha.png");
    carregarTextura(&texturaObstaculoBocha, "texturas/Obstaculo.png");

    modeloBandeiraGolfe = carregarModeloGLB("ModelosGLB/red_flag.glb");
    modeloPinoBoliche = carregarModeloGLB("ModelosGLB/pino_de_boliche__bowling_pin.glb");

    int i;
    for (i = 0; i < 6; i++) {
        carregarTextura(&bolaTextures[i], bolaTextureFiles[i]);
    }
    for (i = 0; i < 6; i++) {
        carregarTextura(&texFundo[i], skyboxFiles[i]);
    }
}

void update(int value) {
    if (gameState == MINIGOLF) {
        bool wasPreviouslyMoving = isAnyBallMoving;
        isAnyBallMoving = false;
        int i, j;
        for (i = 0; i < numPlayers; ++i) {
            if (playerBalls[i].isMoving) {
                Ball* ball = &playerBalls[i];
                float friction = 0.985f;
                ball->pos.x += ball->velocity.x;
                ball->pos.z += ball->velocity.z;
                handleWallCollisions(ball);
                ball->velocity.x *= friction;
                ball->velocity.z *= friction;
                if (fabs(ball->velocity.x) < 0.001f && fabs(ball->velocity.z) < 0.001f) {
                    ball->isMoving = false;
                    ball->velocity.x = 0;
                    ball->velocity.z = 0;
                } else {
                    isAnyBallMoving = true;
                }
            }
        }
        for (i = 0; i < numPlayers; ++i) {
            for (j = i + 1; j < numPlayers; ++j) {
                if (playerBalls[i].inHole || playerBalls[j].inHole) continue;
                Ball* ball1 = &playerBalls[i];
                Ball* ball2 = &playerBalls[j];
                float dx = ball2->pos.x - ball1->pos.x;
                float dz = ball2->pos.z - ball1->pos.z;
                float distance = sqrt(dx * dx + dz * dz);
                if (distance < 0.2f) {
                    if (ball1->isMoving || ball2->isMoving) {
                        playSoundEffect("sons/ball_hit.wav");
                        Vec3 tempVel = ball1->velocity;
                        ball1->velocity = ball2->velocity;
                        ball2->velocity = tempVel;
                        ball1->isMoving = true;
                        ball2->isMoving = true;
                    }
                }
            }
        }
        for (i = 0; i < numPlayers; ++i) {
             if (!playerBalls[i].inHole && fabs(playerBalls[i].pos.x - 3.0f) < 0.2f && fabs(playerBalls[i].pos.z - 3.5f) < 0.2f) {
                playSoundEffect("sons/hole_fall.wav");
                printf("Jogador %d acertou o buraco!\n", i + 1);
                playerBalls[i].inHole = true;
                playerBalls[i].isMoving = false;
                playerBalls[i].pos.y = -1.0f;
            }
        }
        if (wasPreviouslyMoving && !isAnyBallMoving) {
            printf("Todas as bolas pararam de se mover.\n");
            nextTurn();
        }
    }
    else if (gameState == BOWLING) {
        if (!isBallRolling) {
            float sweep_speed = (1.0f + shotPower) * 0.3f;
            if (bowlingAimDirection) {
                bowlingAimAngle += sweep_speed;
                if (bowlingAimAngle > 45.0f) { bowlingAimAngle = 45.0f; bowlingAimDirection = false; }
            } else {
                bowlingAimAngle -= sweep_speed;
                if (bowlingAimAngle < -45.0f) { bowlingAimAngle = -45.0f; bowlingAimDirection = true; }
            }
        }
        if (bowlingBall.isMoving) {
            if (!isInGutter && (bowlingBall.pos.x > 2.0f || bowlingBall.pos.x < -2.0f)) { isInGutter = true; bowlingBall.velocity.x = 0; }
            if(isInGutter){
                float targetY = 0.1f;
                if(bowlingBall.pos.y > targetY){ bowlingBall.pos.y -= 0.05f; }
                if(bowlingBall.pos.x > 0) bowlingBall.pos.x = 2.5f; else bowlingBall.pos.x = -2.5f;
            }
            float friction_z = 0.998f;
            bowlingBall.velocity.z *= friction_z;
            bowlingBall.pos.z += bowlingBall.velocity.z;
            if(!isInGutter){
                 float friction_x = 0.99f;
                 bowlingBall.pos.x += bowlingBall.velocity.x;
                 bowlingBall.velocity.x *= friction_x;
            }
            if (!isInGutter) {
                int i;
                for(i=0; i<10; i++) {
                    if(bowlingPins[i].state == STANDING) {
                        float dx = bowlingPins[i].pos.x - bowlingBall.pos.x;
                        float dz = bowlingPins[i].pos.z - bowlingBall.pos.z;
                        float distance = sqrt(dx*dx + dz*dz);
                        if (distance < 0.55f) {
                            bowlingPins[i].state = FALLING;
                            bowlingPins[i].fallAxis.x = -dz;
                            bowlingPins[i].fallAxis.z = dx;
                            bowlingPins[i].velocity.x = bowlingBall.velocity.x * 0.2f + dx * 0.1f;
                            bowlingPins[i].velocity.z = bowlingBall.velocity.z * 0.5f + dz * 0.1f;
                            if (!hasPlayedPinFallSoundThisTurn) {
                                playSoundEffect("sons/pin_fall.wav");
                                hasPlayedPinFallSoundThisTurn = true;
                            }
                        }
                    }
                }
            }
        }
        int i, j;
        for(i=0; i<10; i++){
            if(bowlingPins[i].state == FALLING) {
                bowlingPins[i].fallAngle += 5.0f;
                if(bowlingPins[i].fallAngle >= 90.0f) { bowlingPins[i].state = FALLEN; bowlingPins[i].velocity.x = 0; bowlingPins[i].velocity.z = 0; }
                bowlingPins[i].pos.x += bowlingPins[i].velocity.x;
                bowlingPins[i].pos.z += bowlingPins[i].velocity.z;
                bowlingPins[i].velocity.x *= 0.95f;
                bowlingPins[i].velocity.z *= 0.95f;
                for(j=0; j<10; j++) {
                    if(i == j) continue;
                    if(bowlingPins[j].state == STANDING) {
                        float dx = bowlingPins[j].pos.x - bowlingPins[i].pos.x;
                        float dz = bowlingPins[j].pos.z - bowlingPins[i].pos.z;
                        float distance = sqrt(dx*dx + dz*dz);
                        if(distance < 0.4f) {
                            bowlingPins[j].state = FALLING;
                            if (!hasPlayedPinFallSoundThisTurn) {
                                playSoundEffect("sons/pin_fall.wav");
                                hasPlayedPinFallSoundThisTurn = true;
                            }
                            bowlingPins[j].fallAxis.x = -dz;
                            bowlingPins[j].fallAxis.z = dx;
                            bowlingPins[j].velocity = bowlingPins[i].velocity;
                        }
                    }
                }
            }
        }
        if (isBallRolling && (fabs(bowlingBall.velocity.z) < 0.001f || bowlingBall.pos.z < -20.0f)) {
            bowlingBall.isMoving = false;
            isBallRolling = false;
            nextBowlingTurn();
        }
    }
    else if (gameState == BOCHA) {
        bool wasPreviouslyMoving = isAnyBochaBallMoving;
        isAnyBochaBallMoving = false;
        int i, j;
        float obstacles[8][4] = {
            {-0.5f, 0.5f, -2.5f, 2.5f}, { 3.5f, 4.5f, -5.5f,-4.5f},
            {-4.5f,-3.5f,  4.5f, 5.5f}, { 7.5f, 8.5f,  7.5f, 8.5f},
            {-8.5f,-7.5f, -8.5f,-7.5f}, { 7.5f, 8.5f, -8.5f,-7.5f},
            {-8.5f,-7.5f,  7.5f, 8.5f}, {-2.5f, 2.5f,  9.5f, 10.5f}
        };
        float ball_radius = 0.2f;

        for (i = 0; i < numPlayers; ++i) {
            if (bochaBalls[i].isMoving && !bochaBalls[i].inHole) {
                Ball* ball = &bochaBalls[i];
                float friction = 0.985f;
                ball->velocity.y -= 0.008f;
                ball->pos.y += ball->velocity.y;
                if (ball->pos.y < ball_radius) {
                    ball->pos.y = ball_radius;
                    ball->velocity.y = 0;
                }
                ball->pos.x += ball->velocity.x;
                ball->pos.z += ball->velocity.z;
                if (ball->pos.y <= ball_radius) {
                    ball->velocity.x *= friction;
                    ball->velocity.z *= friction;
                }
                float arena_limit = 15.0f - ball_radius;
                if (ball->pos.x > arena_limit) { ball->pos.x = arena_limit; ball->velocity.x *= -1; playSoundEffect("sons/wall_hit.wav"); }
                if (ball->pos.x < -arena_limit) { ball->pos.x = -arena_limit; ball->velocity.x *= -1; playSoundEffect("sons/wall_hit.wav"); }
                if (ball->pos.z > arena_limit) { ball->pos.z = arena_limit; ball->velocity.z *= -1; playSoundEffect("sons/wall_hit.wav"); }
                if (ball->pos.z < -arena_limit) { ball->pos.z = -arena_limit; ball->velocity.z *= -1; playSoundEffect("sons/wall_hit.wav"); }

                float ramp_xmin = -0.5f, ramp_xmax = 0.5f;
                float ramp_zmin = -2.5f, ramp_zmax = 2.5f;
                if (ball->pos.x + ball_radius > ramp_xmin && ball->pos.x - ball_radius < ramp_xmax &&
                    ball->pos.z + ball_radius > ramp_zmin && ball->pos.z - ball_radius < ramp_zmax)
                {
                    if (ball->velocity.z < 0 && ball->pos.z > 0 && ball->pos.y <= ball_radius + 0.05) {
                        ball->velocity.y = 0.12f;
                    } else {
                        if (ball->pos.z <= 0) { ball->velocity.z *= -1; playSoundEffect("sons/wall_hit.wav"); }
                        else { ball->velocity.x *= -1; playSoundEffect("sons/wall_hit.wav"); }
                    }
                }
                for(j = 1; j < 8; j++) {
                    float obs_xmin = obstacles[j][0]; float obs_xmax = obstacles[j][1];
                    float obs_zmin = obstacles[j][2]; float obs_zmax = obstacles[j][3];
                    if (ball->pos.x + ball_radius > obs_xmin && ball->pos.x - ball_radius < obs_xmax &&
                        ball->pos.z + ball_radius > obs_zmin && ball->pos.z - ball_radius < obs_zmax)
                    {
                        float overlap_x1 = (ball->pos.x + ball_radius) - obs_xmin;
                        float overlap_x2 = obs_xmax - (ball->pos.x - ball_radius);
                        float overlap_z1 = (ball->pos.z + ball_radius) - obs_zmin;
                        float overlap_z2 = obs_zmax - (ball->pos.z - ball_radius);
                        float min_overlap = fmin(fmin(overlap_x1, overlap_x2), fmin(overlap_z1, overlap_z2));
                        bool hit = false;
                        if (min_overlap == overlap_x1 || min_overlap == overlap_x2) { ball->velocity.x *= -1; hit = true; }
                        if (min_overlap == overlap_z1 || min_overlap == overlap_z2) { ball->velocity.z *= -1; hit = true; }
                        if (hit) playSoundEffect("sons/wall_hit.wav");
                        if(min_overlap == overlap_x1) ball->pos.x = obs_xmin - ball_radius;
                        if(min_overlap == overlap_x2) ball->pos.x = obs_xmax + ball_radius;
                        if(min_overlap == overlap_z1) ball->pos.z = obs_zmin - ball_radius;
                        if(min_overlap == overlap_z2) ball->pos.z = obs_zmax + ball_radius;
                    }
                }
                if (isFirstRoundComplete) {
                    if (i == currentPlayer) {
                        for (j = 0; j < numPlayers; j++) {
                            if (i == j || bochaBalls[j].inHole) continue;
                            Ball* otherBall = &bochaBalls[j];
                            float dx = otherBall->pos.x - ball->pos.x;
                            float dz = otherBall->pos.z - ball->pos.z;
                            float distance = sqrt(dx*dx + dz*dz);
                            if (distance < 0.4f) {
                                playSoundEffect("sons/ball_hit.wav");
                                printf("Jogador %d acertou a bola do Jogador %d!\n", i + 1, j + 1);
                                otherBall->inHole = true;
                                otherBall->isMoving = false;
                                ball->velocity.x = 0;
                                ball->velocity.z = 0;
                                ball->velocity.y = 0;
                                playerGetsAnotherTurn = true;
                            }
                        }
                    }
                }
                if (ball->pos.y <= ball_radius && fabs(ball->velocity.x) < 0.005f && fabs(ball->velocity.z) < 0.005f) {
                    ball->isMoving = false;
                    ball->velocity.x = 0;
                    ball->velocity.z = 0;
                    ball->velocity.y = 0;
                } else {
                    isAnyBochaBallMoving = true;
                }
            }
        }
        if (wasPreviouslyMoving && !isAnyBochaBallMoving) {
            printf("Todas as bolas pararam de se mover.\n");
            nextBochaTurn();
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    if (gameState == PLAYER_SELECTION || gameState == SKIN_SELECTION || gameState == GAME_SELECTION || gameState == END_OF_GAME) {
        glClearColor(0.0f, 0.6f, 0.7f, 1.0f);
    } else {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (gameState) {
        case PLAYER_SELECTION: displayPlayerSelection(); break;
        case SKIN_SELECTION:   displaySkinSelection(); break;
        case GAME_SELECTION:   displayGameSelection(); break;
        case MINIGOLF:         displayMiniGolf(); break;
        case BOWLING:          displayBowling(); break;
        case BOCHA:            displayBocha(); break;
        case END_OF_GAME:      displayEndOfGame(); break;
    }
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27 && gameState != END_OF_GAME) {
        stopBowlingRollLoop();
        stopGrassRollLoop();
        stopBochaRollLoop();
        startMenuMusicLoop();
        gameState = PLAYER_SELECTION;
    }
    switch (gameState) {
        case PLAYER_SELECTION: keyboardPlayerSelection(key, x, y); break;
        case SKIN_SELECTION:   keyboardSkinSelection(key, x, y); break;
        case GAME_SELECTION:   keyboardGameSelection(key, x, y); break;
        case MINIGOLF:         keyboardMiniGolf(key, x, y); break;
        case BOWLING:          keyboardBowling(key, x, y); break;
        case BOCHA:            keyboardBocha(key, x, y); break;
        case END_OF_GAME:      keyboardEndOfGame(key, x, y); break;
    }
}

void specialKey(int key, int x, int y) {
    switch (gameState) {
        case PLAYER_SELECTION: specialKeyPlayerSelection(key, x, y); break;
        case SKIN_SELECTION:   specialKeySkinSelection(key, x, y); break;
        case MINIGOLF:         specialKeyMiniGolf(key, x, y); break;
        case BOWLING:          specialKeyBowling(key, x, y); break;
        case BOCHA:            specialKeyBocha(key, x, y); break;
        default: break;
    }
}

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    float global_ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    float light0_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float light0_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    glEnable(GL_NORMALIZE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    carregarTexturasDoJogo();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fubol Party");

    glutFullScreen();

    srand(time(NULL));
    initAudio();
    startMenuMusicLoop();
    initLighting();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    uninitAudio();
    return 0;
}

void resetGlobalScores() {
    int i;
    for (i = 0; i < MAX_PLAYERS; i++) {
        scores[i] = 0;
    }
}

void startNextGameOrRound() {
    currentRound++;
    if (currentRound > 2) {
        currentRound = 1;
        currentGameIndex++;
    }

    if (currentGameIndex >= 3) {
        gameState = END_OF_GAME;
        printf("FIM DE TODOS OS JOGOS!\n");
    } else {
        gameState = gameSequence[currentGameIndex];
        printf("\n--- Iniciando Rodada %d do Jogo %d ---\n", currentRound, currentGameIndex + 1);
        switch(gameState) {
            case MINIGOLF: initMiniGolf(); break;
            case BOWLING:  initBowling();  break;
            case BOCHA:    initBocha();    break;
            default: break;
        }
    }
}

void drawCenteredText(float y, const char* text) {
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)text);
    float x = (float)(screenWidth - textWidth) / 2.0f;
    drawText(x, y, (char*)text);
}

void drawTexturedQuad(GLuint textureID, float x, float y, float width, float height) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void displayPlayerSelection() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float logoWidth = screenWidth * 0.3f;
    float logoHeight = logoWidth / 1.667f;
    float logoX = (screenWidth - logoWidth) / 2.0f;
    float logoY = screenHeight * 0.50f;
    drawTexturedQuad(logoTextureID, logoX, logoY, logoWidth, logoHeight);

    char playerText[50];
    sprintf(playerText, "Jogadores: %d", numPlayers);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCenteredText(screenHeight * 0.50f, playerText);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawCenteredText(screenHeight * 0.45f, "Use as setas Cima/Baixo para alterar");

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCenteredText(screenHeight * 0.35f, "Pressione ENTER para continuar...");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void keyboardPlayerSelection(unsigned char key, int x, int y) {
    if (key == 13) {
        selectingPlayer = 0;
        selectedSkinIndex = 0;
        int i;
        for(i = 0; i < MAX_PLAYERS; i++) {
            playerTextureSelections[i] = -1;
        }
        gameState = SKIN_SELECTION;
    }
}

void specialKeyPlayerSelection(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        numPlayers++;
        if (numPlayers > MAX_PLAYERS) numPlayers = MAX_PLAYERS;
    }
    if (key == GLUT_KEY_DOWN) {
        numPlayers--;
        if (numPlayers < 2) numPlayers = 2;
    }
}

void displaySkinSelection() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float logoWidth = screenWidth * 0.20f;
    float logoHeight = logoWidth / 1.667f;
    float logoX = (screenWidth - logoWidth) / 2.0f;
    float logoY = screenHeight * 0.72f;
    drawTexturedQuad(logoTextureID, logoX, logoY, logoWidth, logoHeight);

    glColor3f(1.0f, 1.0f, 1.0f);
    char title[100];
    sprintf(title, "Jogador %d, escolha sua skin:", selectingPlayer + 1);
    drawCenteredText(screenHeight * 0.70f, title);

    float verticalSpacing = screenHeight * 0.065f;
    float startY = screenHeight * 0.62f;

    int i;
    for (i = 0; i < 6; i++) {
        char skinText[100];
        bool isTaken = false;
        int j;
        for (j = 0; j < numPlayers; j++) {
            if (playerTextureSelections[j] == i) {
                isTaken = true;
                break;
            }
        }

        if (i == selectedSkinIndex) {
            sprintf(skinText, "> %s <", skinNames[i]);
        } else {
            sprintf(skinText, "  %s  ", skinNames[i]);
        }

        if (isTaken) {
            strcat(skinText, " (Indisponivel)");
            glColor3f(0.5f, 0.5f, 0.5f);
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);
        }

        if (i == selectedSkinIndex) {
            if (isTaken) {
                glColor3f(1.0f, 0.5f, 0.5f);
            } else {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
        }

        drawCenteredText(startY - (i * verticalSpacing), skinText);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCenteredText(screenHeight * 0.20f, "Use Cima/Baixo para navegar e ENTER para confirmar...");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void keyboardSkinSelection(unsigned char key, int x, int y) {
    if (key == 13) {
        bool isTaken = false;
        int i;
        for (i = 0; i < numPlayers; i++) {
            if (playerTextureSelections[i] == selectedSkinIndex) {
                isTaken = true;
                break;
            }
        }

        if (!isTaken) {
            playerTextureSelections[selectingPlayer] = selectedSkinIndex;
            selectingPlayer++;

            while(true) {
                bool isNextTaken = false;
                for (i = 0; i < numPlayers; i++) {
                    if (playerTextureSelections[i] == selectedSkinIndex) {
                        isNextTaken = true;
                        break;
                    }
                }
                if(!isNextTaken) break;
                selectedSkinIndex = (selectedSkinIndex + 1) % 6;
            }

            if (selectingPlayer >= numPlayers) {
                gameState = GAME_SELECTION;
            }
        }
    }
}

void specialKeySkinSelection(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        selectedSkinIndex--;
        if (selectedSkinIndex < 0) selectedSkinIndex = 5;
    }
    if (key == GLUT_KEY_DOWN) {
        selectedSkinIndex++;
        if (selectedSkinIndex > 5) selectedSkinIndex = 0;
    }
}

void displayGameSelection() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float logoWidth = screenWidth * 0.20f;
    float logoHeight = logoWidth / 1.667f;
    float logoX = (screenWidth - logoWidth) / 2.0f;
    float logoY = screenHeight * 0.60f;
    drawTexturedQuad(logoTextureID, logoX, logoY, logoWidth, logoHeight);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCenteredText(screenHeight * 0.60f, "Escolha o primeiro jogo:");

    drawCenteredText(screenHeight * 0.55f, "1. Mini-Golfe");
    drawCenteredText(screenHeight * 0.48f, "2. Boliche");
    drawCenteredText(screenHeight * 0.41f, "3. Bocha");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void keyboardGameSelection(unsigned char key, int x, int y) {
    GameState firstGame = (GameState)-1;
    if (key == '1') firstGame = MINIGOLF;
    if (key == '2') firstGame = BOWLING;
    if (key == '3') firstGame = BOCHA;

    if (firstGame != (GameState)-1) {
        stopMenuMusicLoop();
        resetGlobalScores();
        gameSequence[0] = firstGame;

        GameState remainingGames[2];
        int count = 0;
        if (MINIGOLF != firstGame) remainingGames[count++] = MINIGOLF;
        if (BOWLING != firstGame) remainingGames[count++] = BOWLING;
        if (BOCHA != firstGame) remainingGames[count++] = BOCHA;

        if (rand() % 2 == 0) {
            gameSequence[1] = remainingGames[0];
            gameSequence[2] = remainingGames[1];
        } else {
            gameSequence[1] = remainingGames[1];
            gameSequence[2] = remainingGames[0];
        }

        printf("Ordem dos jogos definida: %d, %d, %d\n", gameSequence[0], gameSequence[1], gameSequence[2]);

        currentGameIndex = 0;
        currentRound = 1;
        gameState = gameSequence[currentGameIndex];

        printf("\n--- Iniciando Rodada %d do Jogo %d ---\n", currentRound, currentGameIndex + 1);

        switch(gameState) {
            case MINIGOLF: initMiniGolf(); break;
            case BOWLING:  initBowling();  break;
            case BOCHA:    initBocha();    break;
            default: break;
        }
    }
}

void desenharFundo(float camX, float camY, float camZ){
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
        glTranslatef(camX, camY, camZ);
        float s = 50.0f;

        glBindTexture(GL_TEXTURE_2D, texFundo[0]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-s,  s, -s);
            glTexCoord2f(0.0f, 1.0f); glVertex3f( s,  s, -s);
            glTexCoord2f(0.0f, 0.0f); glVertex3f( s, -s, -s);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texFundo[1]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( s, -s, s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( s,  s, s);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-s,  s, s);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texFundo[2]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s,  s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(-s,  s,  s);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-s,  s, -s);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texFundo[3]);
        glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(s,  s, -s);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(s,  s,  s);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s,  s);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texFundo[4]);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, s,  s);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( s, s,  s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( s, s, -s);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texFundo[5]);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, -s,  s);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( s, -s,  s);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( s, -s, -s);
        glEnd();

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

void displayEndOfGame() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);

    int winner = -1;
    int maxScore = -1;
    int i;
    for(i = 0; i < numPlayers; i++){
        if(scores[i] > maxScore){
            maxScore = scores[i];
            winner = i;
        }
    }

    drawText(350, 500, "FIM DE JOGO!");
    char winnerText[100];
    sprintf(winnerText, "Vencedor: Jogador %d com %d pontos!", winner + 1, maxScore);
    drawText(250, 450, winnerText);

    drawText(320, 350, "PONTUACOES FINAIS:");
    for(i = 0; i < numPlayers; i++) {
        char scoreText[50];
        sprintf(scoreText, "Jogador %d: %d pontos", i + 1, scores[i]);
        drawText(320, 320 - (i * 30), scoreText);
    }

    drawText(250, 100, "Pressione qualquer tecla para voltar ao menu.");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void keyboardEndOfGame(unsigned char key, int x, int y) {
    startMenuMusicLoop();
    gameState = PLAYER_SELECTION;
}
