#include "MicroBit.h"
#include "SSD1306.h"

#ifndef DISPLAY_NUMBERS_H
#define DISPLAY_NUMBERS_H

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

const ManagedBuffer BYTES_0(WIDTH_0*NUM_HEIGHT) = {0b11111110, 0b11111111, 0b00000001, 0b11111111, 0b11111110,
                                                   0b00000001, 0b00000011, 0b00000010, 0b00000011, 0b00000001};

typedef struct{
    int x;                      // x coordinate of this character
    int y;                      // y coordinate of this character
    int width;                  // width of this character in bytes/pages (eg. 3px = width 3)
    int height;                 // height of this character in bytes/pages (eg. 10px = height 2)
    ManagedBuffer charBytes;    // buffer to store bytes/pages that make up this character
    ManagedBuffer buf;          // buffer to store total byte data of this character
} DisplayCharacter;

DisplayCharacter newCharacter(int width, int height){
    DisplayCharacter ch;
    ch.width = width;
    ch.height = height;
    ch.charBytes = new ManagedBuffer((width*height));

    return ch;
}

DisplayCharacter newCharacter(int width, int height, ManagedBuffer charBytes){
    DisplayCharacter ch;
    ch.width = width;
    ch.height = height;
    ch.charBytes = charBytes;

    return ch;
}

// offset character data to the specified coordinates.
// stores result in ch.buf, which is ready to display.
void offsetCharacter(DisplayCharacter *ch, int x, int y){
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);  // will store all bytes needed to display a character on the screen
    ch->x = x;
    ch->y = y;

    // Find page offset
    int xOffset = x;                            // offset for each row in the horizontal direction
    int yOffset = y % 8 == 0 ? 7 : (y % 8)-1;   // offset for each row in the vertical direction
    int rowOffset = (y - yOffset)/8; ;          // row number of this character

    // populate buffer

    // First byte in the data buffer MUST be 0x40 - this is part of the communication protocol.
    buf[0] = 0x40;

    // initial offset = rowOffset + first xOffset
    int i = 1;
    while (i < rowOffset*OLED_WIDTH + xOffset)
    {
        buf[i] = 0x00;
    }
    
    // character data
    int byteNum = 0;
    // for each row-height+1 because yOffset may push number into another page
    for(int j = 0; j < ch->height+1; j++){
        // for each column
        for (int k = 0; k < ch->width; k++){
            // at any time a page may be made of two parts:
            // the current page, shifted by yOffset(if it exists)
            int part1 = byteNum > ch->charBytes.length() ? 0 :
                        ch->charBytes[byteNum] << yOffset;
            
            // the remainder of the previous page (if it exists)                 
            int part2 = (byteNum - ch->width) < 0 ? 0 :
                        ch->charBytes[byteNum - ch->width] >> (8-yOffset);  
            
            buf[i] = (part1 | part2) & 0xFF;
            i++;
            byteNum++;
        }
        // get to next row
        while (i < OLED_WIDTH - ch->width){
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