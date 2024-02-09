#include "MicroBit.h"
#include "SSD1306.h"

#ifndef DISPLAY_NUMBERS_H
#define DISPLAY_NUMBERS_H

extern MicroBit uBit;

// stores width/height of premade character bytes
enum CharData {
    // width in bytes/pages of each number
    WIDTH_0 = 5,
    WIDTH_1 = 4,
    WIDTH_2 = 5,
    WIDTH_3 = 5,
    WIDTH_4 = 6,
    WIDTH_5 = 5,
    WIDTH_6 = 5,
    WIDTH_7 = 5,
    WIDTH_8 = 5,
    WIDTH_9 = 5,

    // height in bytes/pages of every number
    NUM_HEIGHT = 2,

};

// ---------------------------------------------------------------------------------------------------------------------------------
// premade byte arrays for each number

const uint8_t BYTE_ARR_0[] = {0b11111110, 0b11111111, 0b00000001, 0b11111111, 0b11111110,
                              0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

const uint8_t BYTE_ARR_1[] = {0b00000010, 0b11111111, 0b11111111, 0b00000000,
                              0b00000010, 0b00000011, 0b00000011, 0b00000010};

const uint8_t BYTE_ARR_2[] = {0b11000110, 0b11100111, 0b00110001, 0b00011111, 0b00001110,
                              0b00000011, 0b00000011, 0b00000010, 0b00000010, 0b00000010};

const uint8_t BYTE_ARR_3[] = {0b10000110, 0b10010111, 0b00010001, 0b11111111, 0b11101110,
                              0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

const uint8_t BYTE_ARR_4[] = {0b00110000, 0b00101000, 0b00100100, 0b11111110, 0b11111111, 0b00100000,
                              0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011, 0b00000000};

const uint8_t BYTE_ARR_5[] = {0b10001111, 0b10001111, 0b00001001, 0b11111001, 0b11110001,
                              0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

const uint8_t BYTE_ARR_6[] = {0b11111000, 0b11111110, 0b00010011, 0b11110001, 0b11100000,
                              0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

const uint8_t BYTE_ARR_7[] = {0b00000001, 0b11100001, 0b11111001, 0b00011111, 0b00000111,
                              0b00000000, 0b00000011, 0b00000011, 0b00000000, 0b00000000};

const uint8_t BYTE_ARR_8[] = {0b11101110, 0b11111111, 0b00010001, 0b11111111, 0b11101110,
                              0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

const uint8_t BYTE_ARR_9[] = {0b00011110, 0b00111111, 0b00100001, 0b11111111, 0b11111110,
                              0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000011};

// ---------------------------------------------------------------------------------------------------------------------------------
// premade buffers for each number

const ManagedBuffer BYTES_0((uint8_t*) BYTE_ARR_0, WIDTH_0*NUM_HEIGHT);
const ManagedBuffer BYTES_1((uint8_t*) BYTE_ARR_1, WIDTH_1*NUM_HEIGHT);
const ManagedBuffer BYTES_2((uint8_t*) BYTE_ARR_2, WIDTH_2*NUM_HEIGHT);
const ManagedBuffer BYTES_3((uint8_t*) BYTE_ARR_3, WIDTH_3*NUM_HEIGHT);
const ManagedBuffer BYTES_4((uint8_t*) BYTE_ARR_4, WIDTH_4*NUM_HEIGHT);
const ManagedBuffer BYTES_5((uint8_t*) BYTE_ARR_5, WIDTH_5*NUM_HEIGHT);
const ManagedBuffer BYTES_6((uint8_t*) BYTE_ARR_6, WIDTH_6*NUM_HEIGHT);
const ManagedBuffer BYTES_7((uint8_t*) BYTE_ARR_7, WIDTH_7*NUM_HEIGHT);
const ManagedBuffer BYTES_8((uint8_t*) BYTE_ARR_8, WIDTH_8*NUM_HEIGHT);
const ManagedBuffer BYTES_9((uint8_t*) BYTE_ARR_9, WIDTH_9*NUM_HEIGHT);

// ---------------------------------------------------------------------------------------------------------------------------------
// represents a character which can be displayed on the OLED screen
typedef struct{
    int value;                  // value of this character
    int x;                      // x coordinate of this character
    int y;                      // y coordinate of this character
    int width;                  // width of this character in bytes/pages (eg. 3px = width 3)
    int height;                 // height of this character in bytes/pages (eg. 10px = height 2)
    ManagedBuffer charBytes;    // buffer to store bytes/pages that make up this character
    ManagedBuffer buf;          // buffer to store total byte data of this character - display ready data
} DisplayCharacter;

// ---------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS

// creates and returns a new character, with empty buffers.
DisplayCharacter newCharacter(int value, int width, int height){
    DisplayCharacter ch;
    ManagedBuffer buf((width*height));
    ch.value = value;
    ch.width = width;
    ch.height = height;
    ch.charBytes = buf;

    return ch;
}

// creates and returns a new character, with the given data stored as the visual representation
// of this character.
DisplayCharacter newCharacter(int value, int width, int height, ManagedBuffer charBytes){
    DisplayCharacter ch;
    ch.value = value;
    ch.width = width;
    ch.height = height;
    ch.charBytes = charBytes;

    return ch;
}

// offset character data to the specified coordinates.
// stores result in ch.buf, which is ready to send to the display.
void offsetCharacter(DisplayCharacter *ch, int x, int y){
    // don't allow coordinates out of bounds
    if (x <= 0 || y <= 0 || x > 128 || y > 64){
        uBit.serial.printf("Invalid coordinates\n");
        exit(EXIT_FAILURE);
    }
    
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);  // will store all bytes needed to display a character on the screen
    ch->x = x;
    ch->y = y;

    // Find page offset
    int xOffset = x;                            // offset for each row in the horizontal direction
    int yOffset = y % 8 == 0 ? 8 : (y % 8);     // offset for each row in the vertical direction
    int rowOffset = (y - yOffset)/8;            // row number of this character
    int shiftAmount = yOffset-1;                // the amount to shift data in a page
    // populate buffer

    // First byte in the data buffer MUST be 0x80 - this is part of the communication protocol.
    buf[0] = 0x40;

    // initial offset = rowOffset + first xOffset
    int i = 1;
    while (i < rowOffset*OLED_WIDTH + xOffset)
    {
        buf[i] = 0x00;
        i++;
    }
    // character data
    int byteNum = 0;
    // for each row-height+1 because yOffset may push number into another page
    for(int j = 0; j < ch->height+1; j++){
        // for each column
        for (int k = 0; k < ch->width; k++){
            // at any time a page may be made of two parts:
            // the current page, shifted by yOffset(if it exists)
            int part1 = byteNum >= ch->charBytes.length() ? 0 :
                        ch->charBytes[byteNum] << shiftAmount;
            
            // the remainder of the previous page (if it exists)                 
            int part2 = (byteNum - ch->width) < 0 ? 0 :
                        ch->charBytes[byteNum - ch->width] >> (8-shiftAmount);  
            
            buf[i] = (part1 | part2) & 0xFF;
            i++;
            byteNum++;
            if (i >= buf.length()) break;
        }
        // get to next row
        int checkpoint = i;
        while (i < checkpoint + OLED_WIDTH - ch->width && i < buf.length()){
            buf[i] = 0x00;
            i++;
        }
    }

    // empty space until the end
    while (i < buf.length())
    {
        buf[i] = 0x00;
        i++;
    }

    // store in ch.buf
    ch->buf = buf;
}

#endif