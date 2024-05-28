#include "screen.h"
#include "keyboard.h"
#include <stdlib.h>
#include <unistd.h>

#define PLAYER_UP 'w'
#define PLAYER_DOWN 's'
#define EXIT_KEY 'q'
#define START_KEY 'p'

#define GAME_WIDTH 60
#define GAME_HEIGHT 20

#define BARRIER_SIZE 2

int maxPoints = 10;

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
} Ball;

typedef struct {
    int y;
} Paddle;

typedef struct Barrier {
    int x;
    int y;
    struct Barrier* next;
} Barrier;
#define CPU_SPEED 1
#define BALL_INITIAL_DX 1
#define BALL_INITIAL_DY 1

int barrierMode = 0;

void destroyBarriers(Barrier* barriers) {
    while (barriers != NULL) {
        Barrier* temp = barriers;
        barriers = barriers->next;
        free(temp);
    }
}

void addBarrier(Barrier** barriers, int x, int y) {
    for (int i = 0; i < BARRIER_SIZE; ++i) {
        Barrier* newBarrier = (Barrier*)malloc(sizeof(Barrier));
        if (newBarrier == NULL) {
            fprintf(stderr, "Erro ao alocar memória para a barreira\n");
            exit(EXIT_FAILURE);
        }
        newBarrier->x = x;
        newBarrier->y = y + i;
        newBarrier->next = *barriers;
        *barriers = newBarrier;
    }
}

void drawBarriers(Barrier* barriers) {
    while (barriers != NULL) {
        screenGotoxy(barriers->x, barriers->y);
        printf("|");
        barriers = barriers->next;
    }
}

void checkBarrierCollision(Ball* ball, Barrier** barriers) {
    Barrier* current = *barriers;
    while (current != NULL) {
        if (ball->x == current->x && ball->y >= current->y && ball->y < current->y + BARRIER_SIZE) {
            ball->dx = -ball->dx;
        }
        current = current->next;
    }
}

void initGame(Ball* ball, Paddle* player, Paddle* cpu, Barrier** barriers) {
    ball->x = GAME_WIDTH / 2;
    ball->y = GAME_HEIGHT / 2;
    ball->dx = BALL_INITIAL_DX;
    ball->dy = BALL_INITIAL_DY;

    player->y = GAME_HEIGHT / 2 - 2;
    cpu->y = GAME_HEIGHT / 2 - 2;

    destroyBarriers(*barriers);
    *barriers = NULL;

    if (barrierMode) {
        addBarrier(barriers, GAME_WIDTH / 2, GAME_HEIGHT / 2);
    }
}

void addBarrierAfterPoint(Ball* ball, Barrier** barriers) {
    int barrierX = ball->x + (ball->dx > 0 ? 2 : -2);
    addBarrier(barriers, barrierX, ball->y);
}
void updateBall(Ball* ball, Paddle* player, Paddle* cpu, int* scorePlayer, int* scoreCPU, Barrier** barriers) {
    ball->x += ball->dx;
    ball->y += ball->dy;

    if (ball->y <= 1 || ball->y >= GAME_HEIGHT - 2) {
        ball->dy = -ball->dy;
    }

    if ((ball->x == 3 && ball->dx < 0 && ball->y >= player->y && ball->y < player->y + 4) ||
        (ball->x == GAME_WIDTH - 4 && ball->dx > 0 && ball->y >= cpu->y && ball->y < cpu->y + 4)) {
        if (ball->y >= player->y && ball->y < player->y + 4) {
            ball->dx = -ball->dx;
        } else if (ball->y >= cpu->y && ball->y < cpu->y + 4) {
            ball->dx = -ball->dx;
        }
    }

    checkBarrierCollision(ball, barriers);

    if (ball->x <= 1) {
        (*scoreCPU)++;
        if (barrierMode) {
            addBarrierAfterPoint(ball, barriers);
        }
        initGame(ball, player, cpu, barriers);
    } else if (ball->x >= GAME_WIDTH - 1) {
        (*scorePlayer)++;
        if (barrierMode) {
            addBarrierAfterPoint(ball, barriers);
        }
        initGame(ball, player, cpu, barriers);
    }
}

void updatePlayer(Paddle* player) {
    char key;

    if (keyhit()) {
        key = readch();

        if (key == PLAYER_UP && player->y > 1) {
            player->y--;
        } else if (key == PLAYER_DOWN && player->y + 4 < GAME_HEIGHT) {
            player->y++;
        }

        if (key == EXIT_KEY) {
            screenDestroy();
            keyboardDestroy();
            exit(0);
        }
    }
}

void updateCPU(Paddle* cpu, Ball* ball) {
    if (cpu->y + 2 < ball->y && cpu->y + 4 < GAME_HEIGHT) {
        cpu->y += CPU_SPEED;
    } else if (cpu->y > ball->y && cpu->y > 1) {
        cpu->y -= CPU_SPEED;
    }

    if (rand() % 5 == 0) {
        cpu->y += (rand() % 2 == 0) ? 1 : -1;
    }
}

void drawGame(Ball* ball, Paddle* player, Paddle* cpu, int scorePlayer, int scoreCPU, Barrier** barriers) {
    screenDrawBorders();

    screenGotoxy(ball->x, ball->y);
    printf("o");

    for (int i = 0; i < 4; i++) {
        screenGotoxy(3, player->y + i);
        printf("|");

        screenGotoxy(GAME_WIDTH - 4, cpu->y + i);
        printf("|");
    }

    drawBarriers(*barriers);

    screenGotoxy(GAME_WIDTH / 2 - 2, 1);
    printf("Player: %d", scorePlayer);

    screenGotoxy(GAME_WIDTH / 2 - 2, GAME_HEIGHT);
    printf("CPU: %d", scoreCPU);
}

int showOptionsMenu() {
    screenClear();
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2);
    printf("Pressione 'p' para jogar sem barreiras");
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 + 1);
    printf("Pressione 'b' para jogar com barreiras");
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 + 2);
    printf("Pressione 'q' para sair");
    screenUpdate();
    char key;
    while (1) {
        if (keyhit()) {
            key = readch();
            if (key == START_KEY) {
                barrierMode = 0;
                break;
            } else if (key == 'b') {
                barrierMode = 1;
                break;
            } else if (key == EXIT_KEY) {
                screenDestroy();
                keyboardDestroy();
                exit(0);
            }
        }
    }


    screenClear();
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2);
    printf("Escolha o número de pontos (1-99): ");
    screenUpdate();
    char input[3];
    int numPoints;
    scanf("%2s", input);
    numPoints = atoi(input);
    maxPoints = (numPoints >= 1 && numPoints <= 99) ? numPoints : 10;

    return 1;
}

int showEndMenu(int scorePlayer, int scoreCPU) {
    screenClear();
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 - 1);
    printf("Placar Final:");
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2);
    printf("Jogador: %d", scorePlayer);
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 + 1);
    printf("CPU: %d", scoreCPU);
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 + 3);
    printf("Pressione 'p' para voltar ao menu");
    screenGotoxy(GAME_WIDTH / 2 - 8, GAME_HEIGHT / 2 + 4);
    printf("Pressione 'q' para sair");
    screenUpdate();
    char key;
    while (1) {
        if (keyhit()) {
            key = readch();
            if (key == START_KEY) {
                return 1;
            } else if (key == EXIT_KEY) {
                return 0;
            }
        }
    }
}

int main() {
    int playAgain;

    screenInit(0);
    keyboardInit();

    do {
        Ball ball;
        Paddle player, cpu;
        Barrier* barriers = NULL;
        initGame(&ball, &player, &cpu, &barriers);

        playAgain = showOptionsMenu();

        if (playAgain) {
            int scorePlayer = 0, scoreCPU = 0;
            while (scorePlayer < maxPoints && scoreCPU < maxPoints) {
                updateBall(&ball, &player, &cpu, &scorePlayer, &scoreCPU, &barriers);
                updatePlayer(&player);
                updateCPU(&cpu, &ball);
                drawGame(&ball, &player, &cpu, scorePlayer, scoreCPU, &barriers);
                screenUpdate();
                usleep(50000);
                screenClear();
            }

            destroyBarriers(barriers);

            playAgain = showEndMenu(scorePlayer, scoreCPU);
        }
    } while (playAgain);

    screenDestroy();
    keyboardDestroy();

    return 0;
}
