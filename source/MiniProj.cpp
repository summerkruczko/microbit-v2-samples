#include "MicroBit.h"
#include "KeyBoard.h"
#include "SlidingPuzzle.h"
#include "DisplayLines.h"

extern MicroBit uBit;
SSD1306 *oled;

ManagedBuffer createPuzzleGrid(){
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

void initHardware(){
    uBit.init();
    // set up Microbit to send
    uBit.io.P19.setPull(PullMode::Up);
    uBit.io.P20.setPull(PullMode::Up);
    // Create the display driver
    oled = new SSD1306(OLED_WIDTH, OLED_HEIGHT, 0x78);
}

int main(int argc, char const *argv[])
{
    initHardware();
    ManagedBuffer buf = createPuzzleGrid();
    oled->sendData(buf.getBytes(), buf.length());
    return 0;
}

