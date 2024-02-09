#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef RANDOM_PERMUTATION_H
#define RANDOM_PERMUTATION_H

// method to create a random permutation of integers (lower:higher)
// result is stored in the 1D array provided
void randomPermutation(int* buf, int bsize, int lower, int higher){
    int size = abs(higher - lower) +1;
    int randPerm[size];
    // get nums (lower:higher)
    for(int i = 0; i < size; i++){
        randPerm[i] = lower + i;
    }
    // shuffle
    srand(time(NULL));
    for (int i = size-1; i >= 0; i--){
        int randInd = rand() % (i+1);
        int n = randPerm[i];
        randPerm[i] = randPerm[randInd];
        randPerm[randInd] = n;
    }
    // store bsize integers in buffer
    for (int i = 0; i < bsize; i++)
    {
        buf[i] = randPerm[i];
    }
}

#endif