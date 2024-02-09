#include "MicroBit.h"
#include "KeyBoard.h"
#include "SlidingPuzzle.h"
#include "DisplayLines.h"
#include "DisplayNumbers.h"

MicroBit uBit;
SSD1306 *oled;
KeyPad keyPad;

// information that allows us to display numerical digits
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

// -----------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS TO BUILD THIS SLIDING PUZZLE GAME

// creates a buffer which when sent to the OLED screen, will show a 4x4 grid.
ManagedBuffer createGridBuffer() {
    ManagedBuffer buf;

    // Create and merge vertical lines
    for (int x = 32; x <= 96; x += 16) {
        buf = mergeManagedBuffers(buf, createVerticalLineBuffer(x));
        buf = mergeManagedBuffers(buf, createVerticalLineBuffer(x + 1));
    }

    // Create and merge horizontal lines, with middle ones 2 pixels thick
    for (int y = 1; y <= 65; y += 16) {
        if (y != 1 && y < 64 || y == 65) // half of lines 2-4; line 5
            buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, y - 1));
        else    // half of lines 2-4; line 1
            buf = mergeManagedBuffers(buf, createHorizontalLineBuffer(32, 97, y));
    }

    return buf;
}

// creates a buffer which when sent to the OLED screen, will show the puzzle numbers in
// their respective locations
ManagedBuffer createNumBuffer(SlidingPuzzle p, DisplayCharacter *digits) {
    ManagedBuffer buf;
    ManagedBuffer buf2;
    int xCoordinates[4] = {X_1, X_2, X_3, X_4};
    int yCoordinates[4] = {Y_1, Y_2, Y_3, Y_4};
    int count = 0;
    
    // for each column
    for (int i = 0; i < 4; i++) {
        // for each row: increase count every time as well
        for (int j = 0; j < 4; j++, count++) {
            if (p.puzzleArray[count] == 0) continue; // Don't display 0

            // Handle two-digit numbers (10 to 15)
            if (p.puzzleArray[count] >= 10 && p.puzzleArray[count] <= 15) {
                DisplayCharacter *firstDigit = &ch1;                                // All two-digit numbers handled start with '1'
                DisplayCharacter *secondDigit = &digits[p.puzzleArray[count] % 10]; // eg 13 % 10 = 3
                offsetCharacter(firstDigit, xCoordinates[j], yCoordinates[i]);
                offsetCharacter(secondDigit, xCoordinates[j] + 5, yCoordinates[i]);
                buf2 = mergeManagedBuffers(firstDigit->buf, secondDigit->buf);
            
            // Handle single-digit numbers (1 to 9)
            } else {
                DisplayCharacter *digit = &digits[p.puzzleArray[count]];
                offsetCharacter(digit, xCoordinates[j], yCoordinates[i]);
                buf2 = digit->buf;
            }

            // Merge the individual number buffer into the main buffer
            buf = mergeManagedBuffers(buf, buf2);
        }
    }
    return buf;
}

// takes buffers which display grid/numbers, merges them, and sends to OLED screen.
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

// method to clear the screen
void clearScreen(){
    ManagedBuffer buf(((OLED_WIDTH * OLED_HEIGHT)/8) + 1);
    buf[0] = 0x40;

    // set every page to 0
    for(int i = 1; i <buf.length(); i++){
        buf[i] = 0x00;
    }

    // send to screen
    oled->sendData(buf.getBytes(), buf.length());
}

// sets up hardware needed to play this game
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

// -----------------------------------------------------------------------------------------------------------------------------------------
// MAIN FUNCTION
// run this to play a 4x4 sliding puzzle game!
// press button B to quit (not key B)
int main(int argc, char const *argv[])
{
    // 1 set up componenets
    initHardware();

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
    while (1)
    {
        // quit game
        if (uBit.buttonB.isPressed())
            break;

        // see which keys are pressed
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
            }
        }
        
        // if a tile was slid
        if(tileSlid){
            //  display changes
            displayPuzzle(puzzle);
            // check if puzzle is solved
            if (isSolved(puzzle)){
                uBit.sleep(200);
                // free data
                freePuzzle(&puzzle);
                // create new puzzle
                puzzle = createPuzzle(4, 4);
                // display new puzzle
                displayPuzzle(puzzle);
            }
            
        }
        
        // reset
        tileSlid = 0;
        resetKeys(&keyPad);
    }

    // clear screen
    clearScreen();
    // free data and process
    freePuzzle(&puzzle);
    release_fiber();
    return 0;
}

