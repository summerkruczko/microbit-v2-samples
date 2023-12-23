// #include "MicroBit.h"
// #include "KeyBoard.h"
#include "RandomPermutation.h"

// array to contain the positions of each value when the puzzle is solved
const uint8_t PUZZLE_SOLUTION[16] = {0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8, 9, 10, 11,
                                     12, 13, 14, 15};

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
    uint8_t *puzzleArray;   // the random permutation of values representing shuffled tiles
    uint8_t solved;         // boolean; 0 if this puzzle has not been solved, 1 if it has.
    uint8_t emptySpace;     // index of the empty space within the tile and puzzle array
    Tile **tiles;            // the collection of tiles making up this puzzle.
    
} SlidingPuzzle;

// ------------------------------------------------------------------------------------------------------------------------------------------------

// method to create a random array to represent a puzzle
void createPuzzleArray(uint8_t* buf, uint8_t dim1, uint8_t dim2){
    int randPerm[dim1*dim2];
    // get random permutation of nums (0:max)
    randomPermutation(randPerm, dim1*dim2, 0, dim1*dim2-1);
    // store in puzzle array
    for(int i = 0; i < dim1*dim2; i++){
        buf[i] = (uint8_t) randPerm[i];
        printf("%d ", buf[i]);
    }
    printf("\n");
}

// finds if the specified tile in this sliding puzzle is slideable.
// returns 0 if no, 1 if yes.
int isSlideable(SlidingPuzzle *p, Tile t){
    // if t has value 0, it is the empty space, and is never slideable.
    //printf("assessing tile %d, value %d\n", t.currentInd, t.value);
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

void updateSlideable(SlidingPuzzle *p){
    for (int i = 0; i < p->dim1*p->dim2; i++){
        p->tiles[i]->slideable = isSlideable(p, *p->tiles[i]);
        //printf("Tile slideable %d: %d\n", i, p->tiles[i]->slideable);
    }
}

// creates a new puzzle
SlidingPuzzle createPuzzle(uint8_t dim1, uint8_t dim2){
    SlidingPuzzle p;
    // initialise puzzle
    p.dim1 = dim1;
    p.dim2 = dim2;
    p.puzzleArray = (uint8_t*) malloc(sizeof(uint8_t) * p.dim1 * p.dim2);
    p.tiles = (Tile**) malloc(sizeof(Tile) * p.dim1 * p.dim2);
    createPuzzleArray(p.puzzleArray, p.dim1, p.dim2);
    // printf("puzzle array 0 : %d\n", p.puzzleArray[0]);
    // printf("puzzle array 3 : %d\n", p.puzzleArray[3]);
    p.solved = 0;
    // initialise tiles
    Tile t[p.dim1*p.dim2];
    for (int i = 0; i < p.dim1*p.dim2; i++)
    {
        t[i].value = p.puzzleArray[i];
        t[i].currentX = (i - (i % p.dim1))/dim1;
        t[i].currentY = i % p.dim1;
        t[i].currentInd = i;
        t[i].solvedX = (t[i].value - (t[i].value % p.dim1))/dim1;
        t[i].solvedY = t[i].value % p.dim1;
        t[i].solvedInd = t[i].value;
        p.tiles[i] = &t[i];
        // log position of empty space
        if (t[i].value == 0)
            p.emptySpace = i;
        //printf("Tile value %d: %d\n", i, p.tiles[i]->value);
    }
    // find if each tile is slideable
    updateSlideable(&p);

    return p;
}

// method to slide the specified tile.
// if the tile t is slideable, it will trade places with the empty space et, and 1 will be returned
// otherwise, the tile will not be moved, and 0 will be returned.
int slideTile(SlidingPuzzle *p, Tile *t, Tile *et){
    if (t->slideable == 1){
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

        return 1;
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    SlidingPuzzle puzzle = createPuzzle(3, 3);
    free(puzzle.puzzleArray);
    free(puzzle.tiles);
    return 0;
}