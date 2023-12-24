#include "MicroBit.h"
#include "SSD1306.h"

#ifndef DISPLAY_LINES_H
#define DISPLAY_LINES_H

// Horizontal lines: send n identical bits
ManagedBuffer createHorizontalLineBuffer(uint8_t startX, uint8_t endX, uint8_t y){
    uint8_t bitNum = y % 8;          // get position of line within a row/byte
    if (bitNum == 0) bitNum = 8;     // make
    uint8_t rowY = (y - bitNum)/8;   // get which row the line starts
    uint8_t bitShift = bitNum-1;     // number of times to shift the bit so it appears correctly on the right row

    // First byte in the data buffer MUST be 0x40 - this is part of the communication protocol.
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);
    buf[0] = 0x40;
    // Fill with blank space until start of line
    int i = 1;
    while (i < 128*rowY + startX){
        buf[i] = 0x00;
        i++;
    }
    // Fill with line bits until end of the line or end of the screen
    while (i < endX || i < OLED_WIDTH){
        buf[i] = 1 << bitShift;
        i++;
    }
    // Fill rest of buffer with blank space
    while (i < buf.length()){
        buf[i] = 0x00;
        i++;
    }
    return buf;
}

#endif