/*
 * MIDI_CV_firmware.h
 *
 * Created: 01/04/2014 1:50:02 AM
 *  Author: zhadowoflight
 */ 
/*PIN DEFS*/
#ifndef MAIN_H
#define MAIN_H
#include "Arduino.h"


#define ERROR_LED 8 //TODO: ASSIGN
#define TRIGGER_PIN 2
#define GATE_PIN 3
#define DAC1_SELECT A4
#define DAC2_SELECT A3
#define LCD_SELECT A5
#define IR_SENSOR_PIN_A A7
#define IR_SENSOR_PIN_B A6

/*Buttons*/
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7

/*MIDI CONSTS*/
#define NOTE_ON 9
#define NOTE_OFF 8
#define PITCH_WHEEL 14
#define CONTROL_CHANGE 11


#define IR_ISR_ID 0
#define TRIGGER_ISR_ID 1
#define VCO_ISR_ID 2
#define VCA_ISR_ID 3
#define D2A_ISR_ID 4
#define D2B_ISR_ID 5



uint8_t byteNumber = 0;
#define OMNI_ON true
uint8_t channel = 0;
uint8_t lastMidiMessage = 0;
uint8_t CCType = 0;

uint8_t velocity = 0;
uint8_t note = 0;

uint16_t pitch_wheel = 0x2000;
bool rightChannel = false;
uint8_t messageType;
uint8_t cCVal;

const uint8_t POPULATION = 4;
uint16_t runningAve [POPULATION];
uint16_t runningAve2 [POPULATION];
uint8_t currentIndex = 0;
uint8_t iRcontrolled = 0;
//#define THIRTYTHREE_MV_PER_HALF_TONE
#define  EIGHTYTHREE_MV_PER_HALF_TON
extern uint16_t noteMap [];


//roughly 1 cm appart
uint8_t voltagesPerCm[][14]={
{ //ir0
    230,
    200,
    170,
    150,
    130,
    120,
    110,
    100,
    90,
    95,
    80,
    75,
    70,
    65
},
{ //ir1
    230, 
    200,
    170,
    150,
    130,
    120,
    110,
    100,
    90,
    95,
    80,
    75,
    70,
    65
}
};

/*
    Returns a value from 0 to 14 inclusive 
    based on the ADC voltage roughly 1cm appart
*/
uint8_t noteFromIr (uint8_t voltage,uint8_t irID, uint8_t  * delta )
{
    if(voltage > voltagesPerCm[irID][6])
    {   //lower indices
        
        for(uint8_t ii = 0; ii < 6;++ii)
            if(voltage > voltagesPerCm[irID][ii])
            {   
                *delta = voltage - voltagesPerCm[irID][ii];
                return ii;
            }
        *delta = voltage - voltagesPerCm[irID][6];             
        return 6;
    }
    else
    {   //higher indices than
        for(uint8_t ii = 7; ii < 14;++ii)
            if(voltage > voltagesPerCm[irID][ii])
            {
                *delta = voltage - voltagesPerCm[irID][ii];
                return ii;
            }
        *delta = voltage - voltagesPerCm[irID][14];                
        return 14;
    }
}



#ifdef SIXTEEN_MV_PER_HALF_TONE
//closest power of 2 is 16 so i need to go from 2^14 to 2^4
#define PITCH_ADJUST 10
#define VIBRATO_MAX 16
uint16_t noteMap[] = 
    {
        /*
        var res = "";
        for(var ii = 0 ; ii < 127; ++ii)
        {
        res += " " + Math.round(ii*16.66) +","+"\n";
        }
        console.log(res);
        */
        0,
        17,
        33,
        50,
        67,
        83,
        100,
        117,
        133,
        150,
        167,
        183,
        200,
        217,
        233,
        250,
        267,
        283,
        300,
        317,
        333,
        350,
        367,
        383,
        400,
        417,
        433,
        450,
        466,
        483,
        500,
        516,
        533,
        550,
        566,
        583,
        600,
        616,
        633,
        650,
        666,
        683,
        700,
        716,
        733,
        750,
        766,
        783,
        800,
        816,
        833,
        850,
        866,
        883,
        900,
        916,
        933,
        950,
        966,
        983,
        1000,
        1016,
        1033,
        1050,
        1066,
        1083,
        1100,
        1116,
        1133,
        1150,
        1166,
        1183,
        1200,
        1216,
        1233,
        1250,
        1266,
        1283,
        1299,
        1316,
        1333,
        1349,
        1366,
        1383,
        1399,
        1416,
        1433,
        1449,
        1466,
        1483,
        1499,
        1516,
        1533,
        1549,
        1566,
        1583,
        1599,
        1616,
        1633,
        1649,
        1666,
        1683,
        1699,
        1716,
        1733,
        1749,
        1766,
        1783,
        1799,
        1816,
        1833,
        1849,
        1866,
        1883,
        1899,
        1916,
        1933,
        1949,
        1966,
        1983,
        1999,
        2016,
        2033,
        2049,
        2066,
        2083,
        2099
    };
#endif 
#ifdef THIRTYTHREE_MV_PER_HALF_TONE
//closest power of 2 is 32 so i need to go from 2^14 to 2^5
#define PITCH_ADJUST 9
#define VIBRATO_MAX 32
uint16_t noteMap[] =
{
    0,
    33,
    67,
    100,
    133,
    167,
    200,
    233,
    267,
    300,
    333,
    367,
    400,
    433,
    467,
    500,
    533,
    567,
    600,
    633,
    667,
    700,
    733,
    767,
    800,
    833,
    867,
    900,
    933,
    967,
    1000,
    1033,
    1067,
    1100,
    1133,
    1167,
    1200,
    1233,
    1267,
    1300,
    1333,
    1367,
    1400,
    1433,
    1467,
    1500,
    1533,
    1567,
    1600,
    1633,
    1667,
    1700,
    1733,
    1766,
    1800,
    1833,
    1866,
    1900,
    1933,
    1966,
    2000,
    2033,
    2066,
    2100,
    2133,
    2166,
    2200,
    2233,
    2266,
    2300,
    2333,
    2366,
    2400,
    2433,
    2466,
    2500,
    2533,
    2566,
    2600,
    2633,
    2666,
    2700,
    2733,
    2766,
    2800,
    2833,
    2866,
    2900,
    2933,
    2966,
    3000,
    3033,
    3066,
    3100,
    3133,
    3166,
    3200,
    3233,
    3266,
    3300,
    3333,
    3366,
    3400,
    3433,
    3466,
    3500,
    3533,
    3566,
    3600,
    3633,
    3666,
    3700,
    3733,
    3766,
    3800,
    3833,
    3866,
    3900,
    3933,
    3966,
    4000,
    4033,
    4066,
    4100,
    4133,
    4166,
    4200
};
#endif
#ifdef EIGHTYTHREE_MV_PER_HALF_TON
//closest power of 2 is 64 so i need to go from 2^14 to 2^6
#define PITCH_ADJUST 8
#define VIBRATO_MAX 64
uint16_t noteMap[] =
{
     0,
     83,
     167,
     250,
     333,
     417,
     500,
     583,
     667,
     750,
     833,
     917,
     1000,
     1083,
     1167,
     1250,
     1333,
     1417,
     1500,
     1583,
     1667,
     1750,
     1833,
     1917,
     2000,
     2083,
     2167,
     2250,
     2333,
     2417,
     2500,
     2583,
     2667,
     2750,
     2833,
     2917,
     3000,
     3083,
     3167,
     3250,
     3333,
     3417,
     3500,
     3583,
     3667,
     3750,
     3833,
     3917,
     4000,
     4083,
     4167,
     4250,
     4333,
     4416,
     4500,
     4583,
     4666,
     4750,
     4833,
     4916,
     5000,
     5083,
     5166,
     5250,
     5333,
     5416,
     5500,
     5583,
     5666,
     5750,
     5833,
     5916,
     6000,
     6083,
     6166,
     6250,
     6333,
     6416,
     6500,
     6583,
     6666,
     6750,
     6833,
     6916,
     7000,
     7083,
     7166,
     7250,
     7333,
     7416,
     7500,
     7583,
     7666,
     7750,
     7833,
     7916,
     8000,
     8083,
     8166,
     8250,
     8333,
     8416,
     8500,
     8583,
     8666,
     8750,
     8833,
     8916,
     9000,
     9083,
     9166,
     9250,
     9333,
     9416,
     9500,
     9583,
     9666,
     9750,
     9833,
     9916,
     10000,
     10083,
     10166,
     10250,
     10333,
     10416,
     10500    
};
#endif

#endif
