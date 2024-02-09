#include "MicroBit.h"
#include<stdio.h>

void initTimer(int prescaler, int cc){
    // make sure timer is stopped before configuring bitmode and prescaler
    NRF_TIMER1->TASKS_STOP = 0x1;
    // configure bitmode and prescaler
    NRF_TIMER1->BITMODE = 0x3;          // 32 bit timer bit width
    NRF_TIMER1->PRESCALER = prescaler;  
    // set a CC register
    NRF_TIMER1->CC[0] = cc;
    // set shortcut so timer automatically clears when count reached
    NRF_TIMER1->SHORTS = (1 << 0);
}

// Subtask 1: Bit-Bang Serial
// spins for a period while pin is being set high/low
void spin(){
    // start timer
    NRF_TIMER1->TASKS_START = 0x1;
    // spin until count reached
    while (NRF_TIMER1->EVENTS_COMPARE[0] == 0x0);
    NRF_TIMER1->EVENTS_COMPARE[0] = 0x0;
}

// bit bangs each character
void sendChar(char c){
    // start bits
    NRF_P0->OUT = 0;
    spin();
    // char bits
    for (int i = 0; i < 8; i++){
        if ((c >> i) & 0x1) NRF_P0->OUT = (1 << P0_06) | (1 << P0_02);
        else NRF_P0->OUT = 0;
        spin();
    }
    // end bits
    NRF_P0->OUT = (1 << P0_06) | (1 << P0_02);
    spin(); // spin for a while between characters
    spin();
    spin();
    spin();
}

void bitBangSerial(char* str){
    initTimer(0x4, 0x9);    // prescaler = 4; 9 counts should lead to a rate of ~111111hz
    // set P0.06 high to start
    NRF_P0->DIR = (1 << P0_06) | (1 << P0_02);
    NRF_P0->OUT = (1 << P0_06) | (1 << P0_02);
    // send char
    for (int i = 0; str[i] != '\0'; i++){
        sendChar(str[i]);
    }
}

// -----------------------------------------------------------------------------------------------------------------------

// Subtask 2: Reading Accelerometer Data
// addresses for XYZ output registers
enum Axis {
    X_AXIS_L = 0x28,  // address of OUT_X_L_A
    Y_AXIS_L = 0x2A,  // address of OUT_Y_L_A
    Z_AXIS_L = 0x2C,  // address of OUT_Z_L_A
    X_AXIS_H = 0x29,  // address of OUT_X_H_A
    Y_AXIS_H = 0x2B,  // address of OUT_Y_H_A
    Z_AXIS_H = 0x2D   // address of OUT_Z_H_A
};

// Clear any error flags.
void reset_i2c_flags()
{
    NRF_TWI0->EVENTS_STOPPED = 0;       
    NRF_TWI0->EVENTS_RXDREADY = 0;
    NRF_TWI0->EVENTS_TXDSENT = 0;
    NRF_TWI0->EVENTS_ERROR = 0;
    NRF_TWI0->EVENTS_BB = 0;
    NRF_TWI0->EVENTS_SUSPENDED = 0;
    NRF_TWI0->ERRORSRC = 0xFFFFFFFF;
}

// sets up I2C connection
void initI2C(){
    NRF_TWI0->ENABLE = 0;                   // Disable the TWI peripheral

    NRF_P0->PIN_CNF[P0_08] = 0x00000600;    // Configure SCL pin mode for I2C
    NRF_P0->PIN_CNF[P0_16] = 0x00000600;    // Configure SDA pin mode for I2C

    NRF_TWI0->PSEL.SCL = P0_08;             // Select the pin number associated with the internal I2C SCL line
    NRF_TWI0->PSEL.SDA = P0_16;             // Select the pin number associated with the internal I2C SDA line
    NRF_TWI0->ADDRESS = 0x19;               // I2C Address of accelerometer
    NRF_TWI0->FREQUENCY = 0x04000000;       // Set I2C bus speed to 250kbps
    NRF_TWI0->SHORTS = 1;                   // Suspend the peripheral after every byte. We can then resume it by writing to the TASKS_RESUME register.

    reset_i2c_flags();                      // Always reset flags, so that we can tell when new ones are set :)

    NRF_TWI0->ENABLE = 5;                   // Enable the TWI peripheral
}

// writes to control registers to turn on the accelerometer
void turnOnAccelerometer(){
    initI2C();                  

    NRF_TWI0->TXD = 0x20;                   // Address of CTRL_REG1_A
    NRF_TWI0->TASKS_STARTTX = 1;            // Start an I2C write transaction

    // Wait for data to be sent
    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->EVENTS_TXDSENT = 0;

    reset_i2c_flags();   

    NRF_TWI0->TXD = 0x47;                   // Set register to 0100 0111 (50hz, normal mode, XYZ enabled)
    NRF_TWI0->TASKS_RESUME = 1;             // Resume I2C write transaction

    // Wait for data to be sent
    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->EVENTS_TXDSENT = 0;

    // Finish the transaction
    NRF_TWI0->TASKS_RESUME = 1;
    NRF_TWI0->TASKS_STOP = 1;

    reset_i2c_flags();                      

    NRF_TWI0->TXD = 0x23;                   // Address of CTRL_REG4_A
    NRF_TWI0->TASKS_STARTTX = 1;            // Start an I2C write transaction

    // Wait for data to be sent
    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->EVENTS_TXDSENT = 0;

    reset_i2c_flags();   

    NRF_TWI0->TXD = 0x0;                    // Set register to 0
    NRF_TWI0->TASKS_RESUME = 1;             // Resume I2C write transaction

    // Wait for data to be sent
    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->EVENTS_TXDSENT = 0;

    // Finish the transaction
    NRF_TWI0->TASKS_RESUME = 1;
    NRF_TWI0->TASKS_STOP = 1;
}

// retrieves data from XYZ output registers
int getXYZData(Axis axis){
    uint32_t result = 0;

    initI2C();

    NRF_TWI0->TXD = axis;                   // Address of the X/Y/Z output register   
    NRF_TWI0->TASKS_STARTTX = 1;            // Start an I2C write transaction

    // Wait for data to be sent
    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->EVENTS_TXDSENT = 0;
    
    reset_i2c_flags(); 

    // Start receiving
    NRF_TWI0->TASKS_RESUME = 1;
    NRF_TWI0->TASKS_STARTRX = 1;
   
    // Wait for data to be received
    while(NRF_TWI0->EVENTS_RXDREADY == 0);
    NRF_TWI0->EVENTS_RXDREADY = 0;
   
    // Store the byte that has been received
    result = NRF_TWI0->RXD;

    // Finish the transaction
    NRF_TWI0->TASKS_RESUME = 1;
    NRF_TWI0->TASKS_STOP = 1;

    return result;
}

char* getAccelerometerSample(){
    static char accData[34];
    char xStr[33];
    char yStr[33];
    char zStr[33];

    // set up communication with accelerometer
    initI2C();
    turnOnAccelerometer();

    // get data from output registers
    int x1 = getXYZData(X_AXIS_H);
    int x2 = getXYZData(X_AXIS_L);
    int y1 = getXYZData(Y_AXIS_H);
    int y2 = getXYZData(Y_AXIS_L);
    int z1 = getXYZData(Z_AXIS_H);
    int z2 = getXYZData(Z_AXIS_L);

    // combine values
    int x = (x1 << 2) | x2;
    int y = (y1 << 2) | y2;
    int z = (z1 << 2) | z2;
    int vals[3] = {x, y, z};

    // convert negative values
    for (int i = 0; i < 3; i++){
        if (((vals[i] >> 9) & 0x1) == 1){   // check negative flag at 10th bit
            vals[i] &= 0x1FF;               // get abs value of lower 9 bits
            vals[i] = -vals[i];             // assign negative
        }
    }

    // convert data to strings
    itoa(vals[0], xStr);
    itoa(vals[1], yStr);
    itoa(vals[2], zStr);

    // build result string
    strcpy(accData, "[X: ");
    strcat(accData, xStr);
    strcat(accData, "] [Y: ");
    strcat(accData, yStr);
    strcat(accData, "] [Z: ");
    strcat(accData, zStr);
    strcat(accData, "]\r\n");

    return accData;
}

// print acc sample 5 times a second
void showAccelerometerSample(){
    initTimer(0x4, 0x30D40);    // prescaler = 4, cc = 200000, rate = 5hz
    while(1){
        bitBangSerial(getAccelerometerSample());
        spin();
    }

}