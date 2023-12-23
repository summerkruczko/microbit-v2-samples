#include "MicroBit.h"
#include "KeyBoard.h"

extern MicroBit uBit;

void keyBoardDemo(){
    uBit.init();

    Pin *colPins[] = {&uBit.io.P0, &uBit.io.P1, &uBit.io.P8, &uBit.io.P9};
    Pin *rowPins[] = {&uBit.io.P12, &uBit.io.P2, &uBit.io.P13, &uBit.io.P14};
    KeyPad keyPad = initKeyPad(colPins, rowPins);

    while(1){
        scanKeys(&keyPad);
        if (keyPad.keys[KEY_0].pressed){
            uBit.display.scroll("0");
        }
        if (keyPad.keys[KEY_1].pressed){
            uBit.display.scroll("1");
        }
        if (keyPad.keys[KEY_2].pressed){
            uBit.display.scroll("2");
        }
        if (keyPad.keys[KEY_3].pressed){
            uBit.display.scroll("3");
        }
        if (keyPad.keys[KEY_4].pressed){
            uBit.display.scroll("4");
        }
        if (keyPad.keys[KEY_5].pressed){
            uBit.display.scroll("5");
        }
        if (keyPad.keys[KEY_6].pressed){
            uBit.display.scroll("6");
        }
        if (keyPad.keys[KEY_7].pressed){
            uBit.display.scroll("7");
        }
        if (keyPad.keys[KEY_8].pressed){
            uBit.display.scroll("8");
        }
        if (keyPad.keys[KEY_9].pressed){
            uBit.display.scroll("9");
        }
        if (keyPad.keys[KEY_A].pressed){
            uBit.display.scroll("A");
        }
        if (keyPad.keys[KEY_B].pressed){
            uBit.display.scroll("B");
        }
        if (keyPad.keys[KEY_C].pressed){
            uBit.display.scroll("C");
        }
        if (keyPad.keys[KEY_D].pressed){
            uBit.display.scroll("D");
        }
        if (keyPad.keys[KEY_ASK].pressed){
            uBit.display.scroll("*");
        }
        if (keyPad.keys[KEY_HSH].pressed){
            uBit.display.scroll("#");
        }
        else if (uBit.buttonA.isPressed())
        {
            break;
        }
        resetKeys(&keyPad);
    }
}
