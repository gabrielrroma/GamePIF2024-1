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
