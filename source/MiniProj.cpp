#include "MicroBit.h"
#include "KeyBoard.h"
#include "SlidingPuzzle.h"
#include "DisplayLines.h"
#include "DisplayNumbers.h"

extern MicroBit uBit;
SSD1306 *oled;
KeyPad keyPad;

DisplayCharacter ch0 = newCharacter(0, WIDTH_0, NUM_HEIGHT, BYTES_0);
DisplayCharacter ch1 = newCharacter(1, WIDTH_1, NUM_HEIGHT, BYTES_1);
DisplayCharacter ch2 = newCharacter(2, WIDTH_2, NUM_HEIGHT, BYTES_2);
DisplayCharacter ch3 = newCharacter(3, WIDTH_3, NUM_HEIGHT, BYTES_3);
DisplayCharacter ch4 = newCharacter(4, WIDTH_4, NUM_HEIGHT, BYTES_4);
DisplayCharacter ch5 = newCharacter(5, WIDTH_5, NUM_HEIGHT, BYTES_5);
DisplayCharacter ch6 = newCharacter(6, WIDTH_6, NUM_HEIGHT, BYTES_6);
DisplayCharacter ch7 = newCharacter(7, WIDTH_7, NUM_HEIGHT, BYTES_7);
DisplayCharacter ch8 = newCharacter(8, WIDTH_8, NUM_HEIGHT, BYTES_8);
DisplayCharacter ch9 = newCharacter(9, WIDTH_9, NUM_HEIGHT, BYTES_9);

// maps 'puzzle coordinates' to display coordinates
// eg tile(1,3) would be at point(36, 36).
enum Coordinates {
    X_1 = 36,
    X_2 = 52,
    X_3 = 68,
    X_4 = 84,

    Y_1 = 4,
    Y_2 = 20,
    Y_3 = 36,
    Y_4 = 52,
};

ManagedBuffer createGridBuffer(){
    // Vertical lines. 2 pixels thick
    // V line 1:
    ManagedBuffer buf = createVerticalLineBuffer(32);
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(33));
    // V line 2:
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(48));
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(49));
    // V line 3:
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(64));
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(65));
    // V line 4:
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(80));
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(81));
    // V line 5:
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(96));
    buf = mergeManagedBuffers(buf, createVerticalLineBuffer(97));

    // Horizontal lines. Middle ones are 2 pixels thick
    // H line 1:
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 1));
    // H line 2:
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 17));
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 16));
    // H line 3:
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 32));
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 33));
    // H line 4:
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 48));
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 49));
    // H line 5:
    buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, 64));

    return buf; 
}

// creates a buffer containing display data for all the numbers in a puzzle
ManagedBuffer createNumBuffer(SlidingPuzzle p){
    
}

void initHardware(){
    uBit.init();
    // set up Microbit to send
    uBit.io.P15.setPull(PullMode::Up);
    uBit.io.P16.setPull(PullMode::Up);
    // Create the display driver
    oled = new SSD1306(OLED_WIDTH, OLED_HEIGHT, 0x78);
    // set up keypad
    Pin *colPins[] = {&uBit.io.P0, &uBit.io.P1, &uBit.io.P8, &uBit.io.P9};
    Pin *rowPins[] = {&uBit.io.P12, &uBit.io.P2, &uBit.io.P13, &uBit.io.P14};
    keyPad = initKeyPad(colPins, rowPins);
}

int main(int argc, char const *argv[])
{
    // 1 set up componenets
    initHardware();
    uBit.display.scroll("init");

    // 2 create a new puzzle
    SlidingPuzzle puzzle = createPuzzle(4, 4);

    // 3 display puzzle
    DisplayCharacter digits[10] = {ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9};
    ManagedBuffer gridBuf = createGridBuffer();
    uBit.display.scroll("dig");

    // test grid
    // ManagedBuffer buf = createGridBuffer();

    // test nums
    DisplayCharacter ch = digits[3];
    uBit.display.scroll(digits[3].value);
    offsetCharacter(&ch, 1, 1);
    ManagedBuffer buf2 = ch.buf;
    offsetCharacter(&ch, 15, 19);
    ManagedBuffer buf3 = ch.buf;
    buf2 = mergeManagedBuffers(buf2, buf3);

    oled->sendData(buf2.getBytes(), buf2.length());
    uBit.display.scroll("data sent");
    release_fiber();
    return 0;
}

