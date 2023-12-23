#include "MicroBit.h"

// Subtask 1: Don’t Worry… Be Happy…

// loops 25000 times; is around 50hz
void busyWait(){
    for (volatile int i = 0; i < 25000; i++);
}

void beHappy(){
    while (1){
        // row 1
        NRF_P0->DIR = 0x0;
        NRF_P0->OUT = 0x0;
        busyWait();
        // row 2
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_2) | (1 << COL_2);
        NRF_P0->OUT = (1 << ROW_2);
        busyWait();
        // row 3  
        NRF_P1->DIR = 0x0;
        NRF_P0->DIR = 0x0;
        NRF_P0->OUT = 0x0;
        busyWait();
        // row 4
        NRF_P0->DIR = (1 << ROW_4) | (1 << COL_1) | (1 << COL_5);
        NRF_P0->OUT = (1 << ROW_4);
        busyWait();
        // row 5
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_5) | (1 << COL_2)| (1 << COL_3);
        NRF_P0->OUT = (1 << ROW_5);
        busyWait();
    }
}
// ---------------------------------------------------------------------------------------------------------

// Subtask 2: Using a Hardware Timer

void init50hzTimer(){
    // make sure timer is stopped before configuring bitmode and prescaler
    NRF_TIMER1->TASKS_STOP = 0x1;
    // configure bitmode and prescaler
    NRF_TIMER1->BITMODE = 0x3;      // 32 bit timer bit width
    NRF_TIMER1->PRESCALER = 0x4;    // prescaler = 4; 4000 counts should lead to a rate of ~50hz
    // set a CC register to 4000
    NRF_TIMER1->CC[0] = 0xFA0;
    // set shortcut so timer automatically clears when count reached
    NRF_TIMER1->SHORTS = (1 << 0);
}

void startTimer(){
    // start timer
    NRF_TIMER1->TASKS_START = 0x1;
    // spin until count reached
    while (NRF_TIMER1->EVENTS_COMPARE[0] == 0x0);
    NRF_TIMER1->EVENTS_COMPARE[0] = 0x0;
}

void beVeryHappy(){
    // set up timer
    init50hzTimer();
    
    while (1)
    {
        // row 1
        NRF_P0->DIR = 0x0;
        NRF_P0->OUT = 0x0;
        startTimer();
        // row 2
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_2) | (1 << COL_2);
        NRF_P0->OUT = (1 << ROW_2);
        startTimer();
        // row 3  
        NRF_P1->DIR = 0x0;
        NRF_P0->DIR = 0x0;
        NRF_P0->OUT = 0x0;
        startTimer();
        // row 4
        NRF_P0->DIR = (1 << ROW_4) | (1 << COL_1) | (1 << COL_5);
        NRF_P0->OUT = (1 << ROW_4);
        startTimer();
        // row 5
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_5) | (1 << COL_2)| (1 << COL_3);
        NRF_P0->OUT = (1 << ROW_5);
        startTimer();
    }
}

// ---------------------------------------------------------------------------------------------------------

// Subtask 3: Using Interrupts

// light up the given row 0..4
// draws a smiley face
void showRow(uint8_t rowNum){
    if (rowNum == 0 || rowNum == 2){
        NRF_P1->DIR = 0x0;
        NRF_P0->DIR = 0x0;
        NRF_P0->OUT = 0x0;
    }
    else if (rowNum == 1)
    {
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_2) | (1 << COL_2);
        NRF_P0->OUT = (1 << ROW_2);
    }
    else if (rowNum == 3)
    {
        NRF_P0->DIR = (1 << ROW_4) | (1 << COL_1) | (1 << COL_5);
        NRF_P0->OUT = (1 << ROW_4);
    }
    else if (rowNum == 4)
    {
        NRF_P1->DIR = (1 << 5);
        NRF_P1->OUT = 0x0;
        NRF_P0->DIR = (1 << ROW_5) | (1 << COL_2)| (1 << COL_3);
        NRF_P0->OUT = (1 << ROW_5);
    }
}

// when timer1 reaches 4000, increase row number
void nextRow(){
    if (NRF_TIMER1->EVENTS_COMPARE[0] == 0x1){
        NRF_TIMER1->EVENTS_COMPARE[0] = 0x0;
        // set cc register to current count
        NRF_TIMER2->TASKS_CAPTURE[1] = 0x1;
        // increase count - should clear automatically when it reaches 5
        NRF_TIMER2->TASKS_COUNT = 0x1;
    }
}

void initCounter(int bitMode, int count){
    // stop timer
    NRF_TIMER2->TASKS_STOP = 0x1;
    // confugure bitmode, put in counter mode
    NRF_TIMER2->BITMODE = bitMode;  
    NRF_TIMER2->MODE = 0x2;     // lowpowercounter
    // set cc register
    NRF_TIMER2->CC[0] = count;
    // set shortcut so counter automatically clears when count reached
    NRF_TIMER2->SHORTS = (1 << 0);
}

void beHappyAndFree()
{
    // set up timer
    init50hzTimer();

    // set up counter
    // this will keep track of which row we are lighting up
    // 8 bit; only counting to 5
    // set cc register to 5 for each row
    initCounter(0x1, 0x5);
    
    // set up interrupts
    NRF_TIMER1->INTENSET = 1 << 16;                     // enable interrupts for cc[0]
    NVIC_SetVector(TIMER1_IRQn, (uint32_t)nextRow);
    NVIC_SetPriority(TIMER1_IRQn, 0);
    NVIC_EnableIRQ(TIMER1_IRQn);
    
    // start timers
    NRF_TIMER1->TASKS_START = 0x1;
    NRF_TIMER2->TASKS_START = 0x1;

    // constantly display the current row; interrrupt should change row number
    while (1)
    {
        showRow(NRF_TIMER2->CC[1]);
    }
    
}

// ---------------------------------------------------------------------------------------------------------

// Subtask 4: Controlling the Content

typedef struct {
    uint32_t p0Dir;
    uint32_t p1Dir;
    uint32_t p0Out;

} RowConfiguration;

typedef struct {
    RowConfiguration rows[5];
} LEDConfiguration;

RowConfiguration buildRow(int rowN, int cols[], int colNum){
    RowConfiguration r;
    r.p1Dir = 0;
    r.p0Dir = (1 << rowN);

    for (int i = 0; i < colNum; i++){
        if (cols[i] == COL_4) r.p1Dir = (1 << 5); 
        else r.p0Dir |= (1 << cols[i]);
    }
    
    r.p0Out = (1 << rowN);

    return r;
}

LEDConfiguration buildOne(){
    LEDConfiguration one;
    int cols1[] = {COL_3};
    int cols2[] = {COL_2, COL_3};
    int cols3[] = {COL_2, COL_3, COL_4};

    one.rows[0] = buildRow(ROW_1, cols1, 1);
    one.rows[1] = buildRow(ROW_2, cols2, 2);
    one.rows[2] = buildRow(ROW_3, cols1, 1);
    one.rows[3] = buildRow(ROW_4, cols1, 1);
    one.rows[4] = buildRow(ROW_5, cols3, 3);

    return one;
}

LEDConfiguration buildZero(){
    LEDConfiguration zero;
    int cols1[] = {COL_2, COL_3};
    int cols2[] = {COL_1, COL_4};

    zero.rows[0] = buildRow(ROW_1, cols1, 2);
    zero.rows[1] = buildRow(ROW_2, cols2, 2);
    zero.rows[2] = buildRow(ROW_3, cols2, 2);
    zero.rows[3] = buildRow(ROW_4, cols2, 2);
    zero.rows[4] = buildRow(ROW_5, cols1, 2);

    return zero;
}

LEDConfiguration buildTwo(){
    LEDConfiguration two;
    int cols1[] = {COL_1, COL_2, COL_3};
    int cols2[] = {COL_4};
    int cols3[] = {COL_2, COL_3};
    int cols4[] = {COL_1};
    int cols5[] = {COL_1, COL_2, COL_3, COL_4, COL_5};

    two.rows[0] = buildRow(ROW_1, cols1, 3);
    two.rows[1] = buildRow(ROW_2, cols2, 1);
    two.rows[2] = buildRow(ROW_3, cols3, 2);
    two.rows[3] = buildRow(ROW_4, cols4, 1);
    two.rows[4] = buildRow(ROW_5, cols5, 5);

    return two;
}

LEDConfiguration buildThree(){
    LEDConfiguration three;
    int cols1[] = {COL_1, COL_2, COL_3, COL_4};
    int cols2[] = {COL_4};
    int cols3[] = {COL_3};
    int cols4[] = {COL_1, COL_4};
    int cols5[] = {COL_2, COL_3};

    three.rows[0] = buildRow(ROW_1, cols1, 4);
    three.rows[1] = buildRow(ROW_2, cols2, 1);
    three.rows[2] = buildRow(ROW_3, cols3, 1);
    three.rows[3] = buildRow(ROW_4, cols4, 2);
    three.rows[4] = buildRow(ROW_5, cols5, 2);

    return three;
}

LEDConfiguration buildFour(){
    LEDConfiguration four;
    int cols1[] = {COL_3, COL_4};
    int cols2[] = {COL_2, COL_4};
    int cols3[] = {COL_1, COL_4};
    int cols4[] = {COL_1, COL_2, COL_3, COL_4, COL_5};
    int cols5[] = {COL_4};

    four.rows[0] = buildRow(ROW_1, cols1, 2);
    four.rows[1] = buildRow(ROW_2, cols2, 2);
    four.rows[2] = buildRow(ROW_3, cols3, 2);
    four.rows[3] = buildRow(ROW_4, cols4, 5);
    four.rows[4] = buildRow(ROW_5, cols5, 1);

    return four;
}

LEDConfiguration buildFive(){
    LEDConfiguration five;
    int cols1[] = {COL_1, COL_2, COL_3, COL_4, COL_5};
    int cols2[] = {COL_1};
    int cols3[] = {COL_1, COL_2, COL_3, COL_4};
    int cols4[] = {COL_5};

    five.rows[0] = buildRow(ROW_1, cols1, 5);
    five.rows[1] = buildRow(ROW_2, cols2, 1);
    five.rows[2] = buildRow(ROW_3, cols3, 4);
    five.rows[3] = buildRow(ROW_4, cols4, 1);
    five.rows[4] = buildRow(ROW_5, cols3, 4);

    return five;
}

LEDConfiguration buildSix(){
    LEDConfiguration six;
    int cols1[] = {COL_4};
    int cols2[] = {COL_3};
    int cols3[] = {COL_2, COL_3, COL_4};
    int cols4[] = {COL_1, COL_5};

    six.rows[0] = buildRow(ROW_1, cols1, 1);
    six.rows[1] = buildRow(ROW_2, cols2, 1);
    six.rows[2] = buildRow(ROW_3, cols3, 3);
    six.rows[3] = buildRow(ROW_4, cols4, 2);
    six.rows[4] = buildRow(ROW_5, cols3, 3);

    return six;
}

LEDConfiguration buildSeven(){
    LEDConfiguration seven;
    int cols1[] = {COL_1, COL_2, COL_3, COL_4, COL_5};
    int cols2[] = {COL_4};
    int cols3[] = {COL_3};
    int cols4[] = {COL_2};
    int cols5[] = {COL_1};

    seven.rows[0] = buildRow(ROW_1, cols1, 5);
    seven.rows[1] = buildRow(ROW_2, cols2, 1);
    seven.rows[2] = buildRow(ROW_3, cols3, 1);
    seven.rows[3] = buildRow(ROW_4, cols4, 1);
    seven.rows[4] = buildRow(ROW_5, cols5, 1);

    return seven;
}

LEDConfiguration buildEight(){
    LEDConfiguration eight;
    int cols1[] = {COL_2, COL_3, COL_4};
    int cols2[] = {COL_1, COL_5};

    eight.rows[0] = buildRow(ROW_1, cols1, 3);
    eight.rows[1] = buildRow(ROW_2, cols2, 2);
    eight.rows[2] = buildRow(ROW_3, cols1, 3);
    eight.rows[3] = buildRow(ROW_4, cols2, 2);
    eight.rows[4] = buildRow(ROW_5, cols1, 3);

    return eight;
}

LEDConfiguration buildNine(){
    LEDConfiguration nine;
    int cols1[] = {COL_2, COL_3, COL_4};
    int cols2[] = {COL_1, COL_5};
    int cols3[] = {COL_3};
    int cols4[] = {COL_2};

    nine.rows[0] = buildRow(ROW_1, cols1, 3);
    nine.rows[1] = buildRow(ROW_2, cols2, 2);
    nine.rows[2] = buildRow(ROW_3, cols1, 3);
    nine.rows[3] = buildRow(ROW_4, cols3, 1);
    nine.rows[4] = buildRow(ROW_5, cols4, 1);

    return nine;
}

void showRow2(uint8_t rowNum, LEDConfiguration lc){
    NRF_P1->DIR = lc.rows[rowNum].p1Dir;
    NRF_P0->DIR = lc.rows[rowNum].p0Dir;
    NRF_P0->OUT = lc.rows[rowNum].p0Out;
}

void showNumber(int n){
    LEDConfiguration nums[10] = {buildZero(), buildOne(), buildTwo(), buildThree(), buildFour(),
                                 buildFive(), buildSix(), buildSeven(), buildEight(), buildNine()};
    init50hzTimer();
    initCounter(0x1, 0x5);

    NRF_TIMER1->INTENSET = 1 << 16;                     // enable interrupts for cc[0]
    NVIC_SetVector(TIMER1_IRQn, (uint32_t)nextRow);
    NVIC_SetPriority(TIMER1_IRQn, 0);
    NVIC_EnableIRQ(TIMER1_IRQn);

    NRF_TIMER1->TASKS_START = 0x1;
    NRF_TIMER2->TASKS_START = 0x1;

    while (1)
    {
        showRow2(NRF_TIMER2->CC[1], nums[n]);
    }
}