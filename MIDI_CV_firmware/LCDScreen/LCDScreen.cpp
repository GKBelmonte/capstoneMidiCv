/*
 * LCDScreen.cpp
 *
 * Created: 03/04/2014 3:00:25 AM
 *  Author: zhadowoflight
 */ 


#include <avr/io.h>
#include "..\libs\SPI.h"
#include "..\libs\HardAbs.h"
#include "..\libs\Timers.h"
#include "..\libs\effects.h"
#define F_CPU 32000000
#include <util/delay.h>
uint8_t gate = 0;
void toggleGate()
{
    gate = !gate;
    digitalWrite(PD5,gate);
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,20,1);
}
int main(void)
{
    ClockItUp();
    TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
    TCC0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
    TCC0_HARDABS.SetCompareOrCapture(TimerType0::ChannelA, true);
    TCC0_HARDABS.SetWaveformGenerationMode(TimerType0::SingleSlope);
    TCC0_HARDABS.SetCompareChannelValue8bit(TimerType0::ChannelA,127);
    TCC0_HARDABS.SetTimerDivider(2); //Yields 32MHz / 256 / 256 = 1953 Hz per OF
    pinMode(PD5, OUTPUT);
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,1,1);
    EnableGlobalInterrupts();
    PMIC.CTRL |= 7;
    
    pinMode(PD4, OUTPUT);
    for(uint8_t ii = 0 ; ii< 5;++ii)
    {
        digitalWrite(PD4, LOW);
        _delay_ms(100);
        digitalWrite(PD4, HIGH);
        _delay_ms(100);
        
    }
    
    EnableGlobalInterrupts();
    PMIC.CTRL |= 7;
    for(uint8_t ii = 0 ; ii< 5;++ii)
    {
        digitalWrite(PD4, LOW);
        _delay_ms(200);
        digitalWrite(PD4, HIGH);
        _delay_ms(200);
        
    }
    
    pinMode(PC3,OUTPUT);
    digitalWrite(PC3,HIGH);
    InitalizeSpiStruct();
    SPILib.SetBitOrder(SPI_MSbFIRST);
    SPILib.SetDataMode(SPI_MODE_0_gc);
    SPILib.Begin();
    
    while(1)
    {
        clearLCD();
        _delay_ms(1);
        writeToLCD("Hello\0");
        _delay_ms(1);
    }
    
}