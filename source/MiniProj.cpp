#include "MicroBit.h"
#include "KeyBoard.h"
#include "SlidingPuzzle.h"
#include "DisplayLines.h"
#include "DisplayNumbers.h"

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
    uBit.io.P15.setPull(PullMode::Up);
    uBit.io.P16.setPull(PullMode::Up);
    // Create the display driver
    oled = new SSD1306(OLED_WIDTH, OLED_HEIGHT, 0x78);
}

int main(int argc, char const *argv[])
{
    initHardware();

    // test grid
    // ManagedBuffer buf = createPuzzleGrid();
    // uBit.display.scroll("grid");

    // test nums
    DisplayCharacter ch = newCharacter(WIDTH_0, NUM_HEIGHT, BYTES_0);
    offsetCharacter(&ch, 123, 60);
    ManagedBuffer buf2 = ch.buf;

    // buf = mergeManagedBuffers(buf, buf2);
    // ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);
    // buf[0] = 0x40;
    // for (int i=1; i<buf.length(); i++)
    //     buf[i] = 0x00;
    // for (int i=1; i<ch.charBytes.length(); i++)
    //     buf[i] = ch.charBytes[i];

    oled->sendData(buf2.getBytes(), buf2.length());
    uBit.display.scroll("data sent");
    release_fiber();
    return 0;
}

