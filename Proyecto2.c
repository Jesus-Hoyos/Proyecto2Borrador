#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 8

// Definición de la estructura para cada nodo del tablero
typedef struct Node {
    char piece;
    int value;
    struct Node *north, *south, *east, *west;
} Node;

// Definición de la estructura para los movimientos
typedef struct Move {
    char position[3];
    int value;
    struct Move* next;
} Move;

// Definición de la estructura para el stack
typedef struct Stack {
    Move* move;
    struct Stack* next;
} Stack;

// Función para crear un nuevo nodo
Node* createNode(char piece) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->piece = piece;
    newNode->value = (piece == 'P') ? 1 :
                     (piece == 'A' || piece == 'C') ? 3 :
                     (piece == 'T') ? 5 :
                     (piece == 'Q') ? 8 :
                     (piece == 'K') ? 10 : 0;
    newNode->north = newNode->south = newNode->east = newNode->west = NULL;
    return newNode;
}

// Función para inicializar el tablero
Node*** createBoard() {
    Node ***board = (Node***) malloc(SIZE * sizeof(Node**));
    for (int i = 0; i < SIZE; i++) {
        board[i] = (Node**) malloc(SIZE * sizeof(Node*));
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = createNode('.');
        }
    }
    return board;
}

// Conecta nodos en una malla de 8x8
void linkBoard(Node*** board) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (i > 0) board[i][j]->north = board[i - 1][j];
            if (i < SIZE - 1) board[i][j]->south = board[i + 1][j];
            if (j > 0) board[i][j]->west = board[i][j - 1];
            if (j < SIZE - 1) board[i][j]->east = board[i][j + 1];
        }
    }
}

// Imprime el tablero
void printBoard(Node*** board) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", board[i][j]->piece);
        }
        printf("\n");
    }
}

// Liberar memoria del tablero
void freeBoard(Node*** board) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            free(board[i][j]);
        }
        free(board[i]);
    }
    free(board);
}

// Lee el archivo y llena el tablero
void fillBoard(Node*** board, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error al abrir el archivo.\n");
        return;
    }
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fscanf(file, " %c", &board[i][j]->piece);
            board[i][j]->value = (board[i][j]->piece == 'P') ? 1 :
                                 (board[i][j]->piece == 'A' || board[i][j]->piece == 'C') ? 3 :
                                 (board[i][j]->piece == 'T') ? 5 :
                                 (board[i][j]->piece == 'Q') ? 8 :
                                 (board[i][j]->piece == 'K') ? 10 : 0;
        }
    }
    fclose(file);
}

// Función para añadir un movimiento a la lista
Move* addMove(Move* head, char* position, int value) {
    Move* newMove = (Move*) malloc(sizeof(Move));
    strcpy(newMove->position, position);
    newMove->value = value;
    newMove->next = head;
    return newMove;
}

// Función para encontrar los movimientos de la torre
Move* findMoves(Node*** board, int x, int y) {
    Move* moves = NULL;
    Node* curr;

    // Movimientos hacia el este
    for (int i = y + 1; i < SIZE; i++) {
        curr = board[x][i];
        if (curr->piece != '.') {
            char pos[3] = { 'A' + i, '1' + x, '\0' };
            moves = addMove(moves, pos, curr->value);
            break;
        }
    }
    
    // Movimientos hacia el oeste
    for (int i = y - 1; i >= 0; i--) {
        curr = board[x][i];
        if (curr->piece != '.') {
            char pos[3] = { 'A' + i, '1' + x, '\0' };
            moves = addMove(moves, pos, curr->value);
            break;
        }
    }

    // Movimientos hacia el norte
    for (int i = x - 1; i >= 0; i--) {
        curr = board[i][y];
        if (curr->piece != '.') {
            char pos[3] = { 'A' + y, '1' + i, '\0' };
            moves = addMove(moves, pos, curr->value);
            break;
        }
    }

    // Movimientos hacia el sur
    for (int i = x + 1; i < SIZE; i++) {
        curr = board[i][y];
        if (curr->piece != '.') {
            char pos[3] = { 'A' + y, '1' + i, '\0' };
            moves = addMove(moves, pos, curr->value);
            break;
        }
    }

    return moves;
}

// Imprimir movimientos
void printMoves(Move* moves) {
    int count = 1;
    while (moves) {
        printf("%d. %s - %d\n", count++, moves->position, moves->value);
        moves = moves->next;
    }
}

// Función para apilar un movimiento
void push(Stack** stack, Move* move) {
    Stack* newStackNode = (Stack*) malloc(sizeof(Stack));
    newStackNode->move = move;
    newStackNode->next = *stack;
    *stack = newStackNode;
}

// Función para imprimir y liberar la pila
void printStack(Stack* stack) {
    int count = 1;
    while (stack) {
        printf("%d. %s - %d\n", count++, stack->move->position, stack->move->value);
        Stack* temp = stack;
        stack = stack->next;
        free(temp->move);
        free(temp);
    }
}

// Función principal
int main() {
    Node*** board = createBoard();
    linkBoard(board);
    
    char filename[100];
    printf("Ingrese el nombre del archivo de entrada: ");
    scanf("%s", filename);

    fillBoard(board, filename);
    printBoard(board);

    // Leer las coordenadas de la pieza protagonista (la torre en este caso)
    int x = 3; // Supongamos coordenadas de ejemplo
    int y = 4; // En una posición de ejemplo para la torre

    Move* moves = findMoves(board, x, y);
    Stack* stack = NULL;

    // Ordenar y apilar movimientos en orden de valor
    Move* temp = moves;
    while (temp) {
        if (temp->value > 0) {
            push(&stack, temp);
        }
        temp = temp->next;
    }

    // Imprimir el stack
    printStack(stack);

    // Liberar la memoria del tablero
    freeBoard(board);
    return 0;
}
