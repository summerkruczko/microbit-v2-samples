#include "MicroBit.h"

# define P0 1 << 2
# define P1 1 << 3
# define P2 1 << 4
# define P8 1 << 10
# define P9 1 << 9
# define P11 1 << 23
# define P13 1 << 17
# define P14 1 << 1
# define P15 1 << 13

// Task 1: Turn all the LEDs on
void turnOn(){
    volatile uint32_t *dir = (uint32_t *) 0x50000514;   // direction of GPIO pins
    volatile uint32_t *out = (uint32_t *) 0x50000504;   // write register
    // set pins as output
    *dir = P0 | P1| P2 | P8 | P9 | P11 | P13 | P14;
    // set all channels high
    *out = *dir; 
}

// Task 2: Different bit patterns
void setLEDs(uint8_t value){
    volatile uint32_t *dir = (uint32_t *) 0x50000514;   // direction of GPIO pins
    volatile uint32_t *out = (uint32_t *) 0x50000504;   // write register
    int leds[8] = {P0, P1, P2, P8, P9, P11, P13, P14};

    *dir = 0;
    // isolate each bit; if 1, set corresponding pin channel as output.
    for (int i = 0; i < 8; i++){
        if (value & (1 << (8 -i -1))) *dir |= leds[i];
    }
    
    //set selected channels high
    *out = *dir;
}

// Task 3: Rolling counter
void rollingCounter(){
    volatile uint32_t *dir = (uint32_t *) 0x50000514;     // direction of GPIO pins
    volatile uint32_t *out = (uint32_t *) 0x50000504;     // write register
    int leds[8] = {P0, P1, P2, P8, P9, P11, P13, P14};
    volatile int k = 0;                                   // used in loop for pause

    // loop through increasing count until 255
    for (int count = 0; count <= 255; count++){
        // reset direction every loop
        *dir = 0;

        // isolate each bit; if 1, set corresponding pin channel as output.
        for (int i = 0; i < 8; i++){
            if (count & (1 << (8 -i -1))) *dir |= leds[i];
        }

        //set selected channels high
        *out = *dir;

        // busy wait for pause
        while (k < 64000000/68) {
            k++;
        }
        k = 0;
    }
}

// Task 4: Knight Rider
void knightRider(){
    volatile uint32_t *dir = (uint32_t *) 0x50000514;   // Direction of pins
    volatile uint32_t *out = (uint32_t *) 0x50000504;   // write register
    int leds[8] = {P0, P1, P2, P8, P9, P11, P13, P14};
    volatile int k = 0;                                 // used for loop for pause

    // light up first light before loop - avoids lighting up any led twice in a row
    *dir = leds[0];
    *out = *dir;

    //pause
    while (k < 64000000/68) {
        k++;
    }
    k = 0;

    while(1){
        // go right
        for (int i = 1; i < 8; i++){
            *dir = leds[i];
            *out = *dir;
            // busy wait for pause
            while (k < 64000000/68) {
                k++;
            }
            k = 0;
        }

        // go left
        for (int i = 6; i > -1; i--){
            *dir = leds[i];
            *out = *dir;
            // busy wait for pause
            while (k < 64000000/68) {
                k++;
            }
            k = 0;
        }
    }
}

// Task 5: Count the number of button clicks
// doesn't work :(
void countClicks(){
    volatile uint32_t *dir = (uint32_t *) 0x50000514;   // direction of GPIO pins
    volatile uint32_t *out = (uint32_t *) 0x50000504;   // write register
    volatile uint32_t *in = (uint32_t *) 0x50000510;    // read register
    volatile uint32_t *cnf = (uint32_t *) 0x5000073C;   // configure input for P15
    int leds[8] = {P0, P1, P2, P8, P9, P11, P13, P14};
    int count = 0;

    *dir = 0;   // make sure bit 13 is low, so we can take input
    *out = 0;
    *cnf = 0;   // clear bit 1, enable input

    while(1){
        // when bit 13 is high button NOT PRESSED: do nothing
        if (*in & P15){} 
        
        // when bit 13 low button PRESSED: increase count
        else {
            count ++;
            // display count in binary
            // isolate each bit; if 1, set corresponding pin channel as output.
            for (int i = 0; i < 8; i++){
                if (count & (1 << (8 -i -1))) *dir |= leds[i];
            }
            *out = *dir; // light up LEDs
            *dir = 0;    // reset direction
        }
    }
}