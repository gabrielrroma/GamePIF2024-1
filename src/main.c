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
