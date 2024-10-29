#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 8

typedef struct Node {
    char piece;
    int value;
    struct Node *north, *south, *east, *west;
} Node;

typedef struct Move {
    char position[3];
    int value;
    struct Move* next;
} Move;

typedef struct Stack {
    Move* move;
    struct Stack* next;
} Stack;