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
ManagedBuffer createNumBuffer(SlidingPuzzle p, DisplayCharacter *digits){
    ManagedBuffer buf;
    ManagedBuffer buf2;
    int xCoordinates[4] = {X_1, X_2, X_3, X_4};
    int yCoordinates[4] = {Y_1, Y_2, Y_3, Y_4};
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (p.puzzleArray[count] == 0);     // don't display
            // in the case of 2 digit numbers
            else if (p.puzzleArray[count] == 10){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(&ch0, xCoordinates[j]+5, yCoordinates[i]);
                buf2 = mergeManagedBuffers(ch1.buf, ch0.buf);
            }
            else if (p.puzzleArray[count] == 11){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                buf2 = ch1.buf;
                offsetCharacter(&ch1, xCoordinates[j]+6, yCoordinates[i]);
                buf2 = mergeManagedBuffers(buf2, ch1.buf);
            }
            else if (p.puzzleArray[count] == 12){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(&ch2, xCoordinates[j]+5, yCoordinates[i]);
                buf2 = mergeManagedBuffers(ch1.buf, ch2.buf);
            }
            else if (p.puzzleArray[count] == 13){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(&ch3, xCoordinates[j]+5, yCoordinates[i]);
                buf2 = mergeManagedBuffers(ch1.buf, ch3.buf);
            }
            else if (p.puzzleArray[count] == 14){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(&ch4, xCoordinates[j]+4, yCoordinates[i]);
                buf2 = mergeManagedBuffers(ch1.buf, ch4.buf);
            }
            else if (p.puzzleArray[count] == 15){
                offsetCharacter(&ch1, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(&ch5, xCoordinates[j]+5, yCoordinates[i]);
                buf2 = mergeManagedBuffers(ch1.buf, ch5.buf);
            }
            // single digit numbers except 0
            else if (p.puzzleArray[count] < 10){
                offsetCharacter(&digits[p.puzzleArray[count]], xCoordinates[j], yCoordinates[i]);
                buf2 = digits[p.puzzleArray[count]].buf;
            }
            // merge buffers
            buf = mergeManagedBuffers(buf, buf2);  
            count++;
        }
    }
    return buf;
}

void displayPuzzle(SlidingPuzzle p){
    DisplayCharacter digits[10] = {ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, ch9};

    // get buffers
    ManagedBuffer gridBuf = createGridBuffer();
    ManagedBuffer numBuf = createNumBuffer(p, digits);
    // merge
    ManagedBuffer buf = mergeManagedBuffers(gridBuf, numBuf);

    // display
    oled->sendData(buf.getBytes(), buf.length());
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
    displayPuzzle(puzzle);

    // start game
    int keyNums[16] = {KEY_1, KEY_2, KEY_3, KEY_A,
                       KEY_4, KEY_5, KEY_6, KEY_B, 
                       KEY_7, KEY_8, KEY_9, KEY_C,
                       KEY_ASK, KEY_0, KEY_HSH, KEY_D};
    int tileSlid = 0;
    
    // while the game is running:
    while (uBit.buttonB.isPressed() == 0)
    {
        scanKeys(&keyPad);

        // check each key
        for (int i = 0; i < 16; i++)
        {
            // if a key is pressed
            if (keyPad.keys[keyNums[i]].pressed){
                // if corresponding tile is slideable
                if(puzzle.tiles[i]->slideable){
                    // slide tile
                    slideTile(&puzzle, puzzle.tiles[i], puzzle.tiles[puzzle.emptySpace]);
                    tileSlid = 1;
                } 
                // otherwise, make some noise
                else {
                    //uBit.audio.soundExpressions.playAsync("twinkle");
                    uBit.display.printChar('x');
                }
            }
        }
        

        // if a tile was slid
        if(tileSlid){
            //  display changes
            displayPuzzle(puzzle);
            // check if puzzle is solved
            if (isSolved(puzzle)){
                uBit.sleep(200);
                freePuzzle(&puzzle);
                puzzle = createPuzzle(4, 4);
                displayPuzzle(puzzle);
            }
        }
        
        // reset
        tileSlid = 0;
        resetKeys(&keyPad);
    }
    

    release_fiber();
    return 0;
}

