#include "MicroBit.h"
#include "RandomPermutation.h"

#ifndef SLIDING_PUZZLE_H
#define SLIDING_PUZZLE_H

extern MicroBit uBit;

// array to contain the positions of each value when the puzzle is solved
// const uint8_t PUZZLE_SOLUTION[16] = {0, 1, 2, 3,
//                                      4, 5, 6, 7,
//                                      8, 9, 10, 11,
//                                      12, 13, 14, 15};

// defines a sliding tile within a sliding puzzle
typedef struct {
    uint8_t value;      // the number on this tile (0 if the empty space)
    uint8_t currentX;   // the current x position of this tile
    uint8_t currentY;   // the current y position of this tile
    uint8_t currentInd; // the current index of this tile
    uint8_t solvedX;    // the x position of this tile in the solution
    uint8_t solvedY;    // the y position of this tile in the solution
    uint8_t solvedInd;  // the index of this tile in the solution
    uint8_t slideable;  // boolean; 0 if not slideable, 1 if can be moved. Tile with value = 0 is never slideable.
} Tile;

// defines a sliding puzzle
typedef struct {
    uint8_t dim1;           // number of rows in the sliding puzzle 
    uint8_t dim2;           // number of columns in the sliding puzzle 
    uint8_t *solvedArray;   // what the puzzle looks like when solved    
    uint8_t *puzzleArray;   // the random permutation of values representing shuffled tiles
    uint8_t solved;         // boolean; 0 if this puzzle has not been solved, 1 if it has.
    uint8_t emptySpace;     // index of the empty space within the tile and puzzle array
    Tile **tiles;           // the collection of tiles making up this puzzle.
    
} SlidingPuzzle;

// ------------------------------------------------------------------------------------------------------------------------------------------------
// METHODS
// Function to find the position of the blank (0) tile
int findBlankPosition(uint8_t* puzzle, int dim1, int dim2) {
    for (int i = 0; i < dim1 * dim2; i++) {
        if (puzzle[i] == 0) {
            return i;
        }
    }
    return -1; // Should never happen if the puzzle is valid
}

// Function that takes two arrays of same length and checks they are equal
int arraysEqual(uint8_t* arr1, uint8_t* arr2, int length) {
    for (int i = 0; i < length; i++) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }
    return 1;
}

// Function that checks if the puzzle is solved
// Returns 1 if yes, 0 if no
int isSolved(SlidingPuzzle p){
    if (arraysEqual(p.solvedArray, p.puzzleArray, p.dim1*p.dim2))
        return 1;
    else 
        return 0;
}

// Create the solution array for a puzzle
void createSolutionArray(uint8_t* buf, uint8_t dim1, uint8_t dim2) {
    for (int i = 0; i < dim1 * dim2; i++) {
        buf[i] = i;
    }
}


// finds if the specified tile in this sliding puzzle is slideable.
// returns 0 if no, 1 if yes.
int isSlideable(SlidingPuzzle *p, Tile t){
    // if t has value 0, it is the empty space, and is never slideable.
    // printf("assessing tile %d, value %d\n", t.currentInd, t.value);
    if (t.value == 0)
        return 0;
    // find adjacent tiles
    for (int i = 0; i < p->dim1*p->dim2; i++){
        Tile t2 = *p->tiles[i];
        // adjacent tiles have either (same x, y+-1) or (same y, x+-1)
        if ((t2.currentX == t.currentX && (t2.currentY == t.currentY+1 || t2.currentY == t.currentY-1)) ||
            (t2.currentY == t.currentY && (t2.currentX == t.currentX+1 || t2.currentX == t.currentX-1))){
            // if adjacent tile has value 0, it is the empty space, and this tile is slideable.
            if (t2.value == 0){
                // may as well log position while we've found it :)
                p->emptySpace = i;
                return 1;
            }
        }
    }
    return 0;
}

// updates slideable property for every tile in the given puzzle
void updateSlideable(SlidingPuzzle *p){
    for (int i = 0; i < p->dim1*p->dim2; i++){
        p->tiles[i]->slideable = isSlideable(p, *p->tiles[i]);
        // printf("Tile slideable %d: %d\n", i, p->tiles[i]->slideable);
    }
}

// method to slide the specified tile.
// if the tile t is slideable, it will trade places with the empty space et, and 1 will be returned
// otherwise, the tile will not be moved, and 0 will be returned.
int slideTile(SlidingPuzzle *p, Tile *t, Tile *et){
    if (t->slideable == 1 && et->value == 0){
        // store tile values
        Tile movedTile = *t;

        // swap positions
        t->currentInd = et->currentInd;
        t->currentX = et->currentX;
        t->currentY = et->currentY;
        et->currentInd = movedTile.currentInd;
        et->currentX = movedTile.currentX;
        et->currentY = movedTile.currentY;

        // update puzzle
        p->puzzleArray[t->currentInd] = t->value;
        p->tiles[t->currentInd] = t;
        p->puzzleArray[et->currentInd] = et->value;
        p->tiles[et->currentInd] = et;
        p->emptySpace = et->currentInd;

        // update slideable
        updateSlideable(p);

        return 1;
    }
    return 0;
}

// creates a new puzzle
SlidingPuzzle createPuzzle(uint8_t dim1, uint8_t dim2){
    // Validate dimensions
    if (dim1 <= 0 || dim2 <= 0) {
        fprintf(stderr, "Invalid dimensions for the puzzle\n");
        exit(EXIT_FAILURE);
    }
    // uBit.display.scroll("1");
    SlidingPuzzle p;
    // initialise puzzle
    p.dim1 = dim1;
    p.dim2 = dim2;

    // try to allocate space to the arrays
    p.puzzleArray = (uint8_t*) malloc(sizeof(uint8_t) * p.dim1 * p.dim2);
    if (!p.puzzleArray) {
        fprintf(stderr, "Memory allocation failed for puzzleArray\n");
        exit(EXIT_FAILURE);
    }
    // uBit.display.scroll("2");

    p.solvedArray = (uint8_t*) malloc(sizeof(uint8_t) * p.dim1 * p.dim2);
    if (!p.solvedArray) {
        fprintf(stderr, "Memory allocation failed for puzzleArray\n");
        free(p.puzzleArray);  // Clean up previously allocated memory
        exit(EXIT_FAILURE);
    }
    // uBit.display.scroll("3");

    p.tiles = (Tile**) malloc(sizeof(Tile*) * p.dim1 * p.dim2);
    if (!p.tiles) {
        fprintf(stderr, "Memory allocation failed for tiles\n");
        free(p.puzzleArray);  // Clean up previously allocated memory
        free(p.solvedArray);
        exit(EXIT_FAILURE);
    }
    // uBit.display.scroll("4");
    // uBit.display.scroll("5");
    createSolutionArray(p.solvedArray, p.dim1, p.dim2);
    // uBit.display.scroll("6");
    
    // copy the solution array to the puzzle array
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        p.puzzleArray[i] = p.solvedArray[i];
    }
    p.solved = 0;

    // initialise tiles
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        // uBit.display.scroll("7");
        Tile* t = (Tile*) malloc(sizeof(Tile)); // dynamically allocate each tile
        if (!t) {
            fprintf(stderr, "Memory allocation failed for tile %d\n", i);
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(p.tiles[j]);
            }
            free(p.tiles);
            free(p.puzzleArray);
            free(p.solvedArray);
            exit(EXIT_FAILURE);
        }
        // uBit.display.scroll("8");
        t->value = p.puzzleArray[i];
        t->currentX = (i - (i % p.dim1))/dim1;
        t->currentY = i % p.dim1;
        t->currentInd = i;
        t->solvedX = (t->value - (t->value % p.dim1))/dim1;
        t->solvedY = t->value % p.dim1;
        t->solvedInd = t->value;
        p.tiles[i] = t;  // Store the pointer to the dynamically allocated tile
        // log position of empty space
        if (t->value == 0)
            p.emptySpace = i;
    }
    // find if each tile is slideable
    updateSlideable(&p);
    // uBit.display.scroll("9");

    // Now we shuffle the tiles to create a solvable puzzle
    // We do this by sliding the empty space around the puzzle
    // We do this a random number of times
    int numMoves = 100;
    srand(time(NULL));
    for (int i = 0; i < numMoves; i++) {
        // Find the empty space
        Tile* emptyTile = p.tiles[p.emptySpace];
        // Find a random adjacent tile
        
        int randomTile = rand() % (p.dim1 * p.dim2);
        while (!isSlideable(&p, *p.tiles[randomTile])) {
            randomTile = rand() % (p.dim1 * p.dim2);
        }
        // Slide the random tile into the empty space
        slideTile(&p, p.tiles[randomTile], emptyTile);
    }
    // uBit.display.scroll("10");
    return p;
}

// frees memory allocated to specified puzzle
void freePuzzle(SlidingPuzzle* p){
    // Free each dynamically allocated tile
    for (int i = 0; i < p->dim1 * p->dim2; i++){
        free(p->tiles[i]);
    }
    // Free the array of pointers to tiles
    free(p->tiles);
    // Free the puzzle array
    free(p->puzzleArray);
    // Free the solved array
    free(p->solvedArray);

    // Set the pointer to NULL to indicate that the memory has been freed
    p->tiles = NULL;
    p->puzzleArray = NULL;
    p->solvedArray = NULL;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------
//DEBUGGING

void printTile(Tile *t){
    printf("Value: %d\n", t->value);
    printf("CurrentX: %d\n", t->currentX);
    printf("CurrentY: %d\n", t->currentY);
    printf("CurrentInd: %d\n", t->currentInd);
    printf("SolvedX: %d\n", t->solvedX);
    printf("SolvedY: %d\n", t->solvedY);
    printf("SolvedInd: %d\n", t->solvedInd);
    printf("Slideable: %d\n", t->slideable);
}

void printPuzzle(SlidingPuzzle p){
    printf("Dim1: %d\n", p.dim1);
    printf("Dim2: %d\n", p.dim2);
    printf("Solved: %d\n", p.solved);
    printf("EmptySpace: %d\n", p.emptySpace);
    printf("PuzzleArray: ");
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        printf("%d ", p.puzzleArray[i]);
    }
    printf("\nTiles:\n");
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        printf("Tile %d:\n", i);
        printTile(p.tiles[i]);
        printf("\n");
    }
}

void printPuzzleASCII(SlidingPuzzle p){

    printf("\n Current Puzzle:\n");
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        printf("%d ", p.puzzleArray[i]);
        if ((i+1) % p.dim1 == 0)
            printf("\n");
    }
    printf("\n Goal Puzzle:\n");
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        printf("%d ", p.solvedArray[i]);
        if ((i+1) % p.dim1 == 0)
            printf("\n");
    }
}

#endif