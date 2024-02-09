#include "MicroBit.h"

#ifndef KEYBOARD_H
#define KEYBOARD_H

// represents a key on the keypad
typedef struct{
    Pin *colPin;    // the pin connected to the column for this key
    Pin *rowPin;    // the pin connected to the row for this key
    int pressed;    // whether the key has been pressed: 0 - no, 1 - yes
} Key;

// represents the 4x4 keypad
typedef struct{
    Pin *colPins[4];    // the pins connected to the columns for this keypad
    Pin *rowPins[4];    // the pins connected to the rows for this keypad
    Key keys[16];       // the keys on this keypad
} KeyPad;

// maps each key to its index within the keys array
enum KeyType {
    KEY_0 = 7,
    KEY_1 = 0,
    KEY_2 = 4,
    KEY_3 = 8,
    KEY_4 = 1,
    KEY_5 = 5,
    KEY_6 = 9,
    KEY_7 = 2,
    KEY_8 = 6,
    KEY_9 = 10,

    KEY_A = 12,
    KEY_B = 13,
    KEY_C = 14,
    KEY_D = 15,

    KEY_ASK = 3,
    KEY_HSH = 11
};

// -----------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS

// method to find the state of each key
// feeds power through each column, and finds which rows the power is being channelled to;
// the corresponding keys have their pressed attribute set to 1.
void scanKeys(KeyPad *keyPad){
    int keyNo = 0;
    for (int i = 0; i < 4; i++){
        // feed power through each column
        keyPad->colPins[i]->setDigitalValue(1);

        // read state of each row
        for (int j = 0; j < 4; j++){
            // if row high, key = pressed
            if (keyPad->rowPins[j]->getDigitalValue()){
                keyPad->keys[keyNo].pressed = 1;
            } else 
                keyPad->keys[keyNo].pressed = 0;
            keyNo++;
        }
        // reset column
        keyPad->colPins[i]->setDigitalValue(0);
    }
}

// method to reset the 'pressed' state of each key
void resetKeys(KeyPad *keyPad){
    for (int i = 0; i < 16; i++)
    {
        keyPad->keys[i].pressed = 0;
    }
}

// method to initialise a keypad
// takes 2 arrays as parameter; each must have at least 4 items
    // colPins = the pins connected to each column
    // rowPins = the pins connected to each row
KeyPad initKeyPad(Pin *colPins[], Pin *rowPins[]){
    KeyPad keyPad;
    int keyNo = 0;
    
    for (int i = 0; i < 4; i++)
    {
        // store pins used
        keyPad.colPins[i] = colPins[i];
        keyPad.rowPins[i] = rowPins[i];

        // store pin combinations for each key, and set each as not pressed
        for (int j = 0; j < 4; j++)
        {
            keyPad.keys[keyNo].colPin = colPins[i];
            keyPad.keys[keyNo].rowPin = rowPins[j];
            keyPad.keys[keyNo].pressed = 0;
            keyNo++;
        }

        // initialise columns to 0
        keyPad.colPins[i]->setDigitalValue(0);
    }
    
    return keyPad;
}

#endif