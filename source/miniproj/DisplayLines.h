#include "MicroBit.h"
#include "SSD1306.h"

#ifndef DISPLAY_LINES_H
#define DISPLAY_LINES_H

// method to draw horizontal lines at y from startX to endX on the display.
// result returned in ManagedBuffer object.
ManagedBuffer createHorizontalLineBuffer(uint8_t startX, uint8_t endX, uint8_t y){
    uint8_t bitNum = y % 8 == 0 ? 8 : y % 8;    // get position of line within a row/byte, 1:8. Make sure it doesn't equal 0
    uint8_t rowOffset = (y - bitNum)/8;         // get which row the line starts
    uint8_t bitShift = bitNum-1;                // number of times to shift the bit so it appears correctly on the right row

    // First byte in the data buffer MUST be 0x80 - this is part of the communication protocol.
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);
    buf[0] = 0x40;
    // Fill with blank space until start of line
    int i = 1;
    while (i < OLED_WIDTH*rowOffset + startX){
        buf[i] = 0x00;
        i++;
    }
    // Fill with line bits until end of the line or end of the screen
    while (i <= OLED_WIDTH*rowOffset + endX && i <= OLED_WIDTH*rowOffset + OLED_WIDTH){
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

// method to draw vertical lines at x from start to the end of the display.
// for puzzle purposes, x values not necessary!
// result returned in ManagedBuffer object.
ManagedBuffer createVerticalLineBuffer(uint8_t x){
    // First byte in the data buffer MUST be 0x80 - this is part of the communication protocol.
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);
    buf[0] = 0x40;
    // First bit of empty space
    int i = 1;
    while (i < x)
    {
        buf[i] = 0x00;
        i++;
    }
    // Then loop alternating empty space (x128) with line bytes
    int count = 0;  // number alternations
    while (i<buf.length())
    {
        // when it should draw a line
        if (i == x + count*OLED_WIDTH){
            buf[i] = 0xFF;
            count++;
        }
        // empty space
        else buf[i] = 0x00;
        i++;
    }

    return buf;
}

// merges 2 buffers together, and returns the result
ManagedBuffer mergeManagedBuffers(ManagedBuffer buffer1, ManagedBuffer buffer2){
    // Determine the maximum length of the two buffers
    int maxLength = buffer1.length() > buffer2.length() ? buffer1.length() : buffer2.length();

    // Create a new ManagedBuffer to store the result
    ManagedBuffer result(maxLength);

    // Iterate through each byte in the buffers
    for (int i = 0; i < maxLength; i++) {
        uint8_t byte1 = i < buffer1.length() ? buffer1[i] : 0;
        uint8_t byte2 = i < buffer2.length() ? buffer2[i] : 0;

        // Perform the bitwise OR operation and store the result
        result[i] = byte1 | byte2;
    }

    return result;
}


#endif