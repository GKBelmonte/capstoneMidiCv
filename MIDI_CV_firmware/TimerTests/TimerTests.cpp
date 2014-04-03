/*
 * TimerTests.cpp
 *
 * Created: 26/03/2014 2:50:33 AM
 *  Author: zhadowoflight
 */ 

#include <avr/io.h>
#include "..\libs\Timers.h"
#include "..\libs\HardAbs.h"
#define F_CPU 32000000UL
#include <util/delay.h>
#define TRIGGER_PIN PD4
#define GATE_PIN PD5

bool trigger = false;
void toggleTrigger()
{
    trigger = !trigger;
    digitalWrite(TRIGGER_PIN,trigger);
    TCC0_HARDABS.CallMeBackInNOverflows(toggleTrigger,10,0);
}
bool gate = false;
void toggleGate()
{
    gate = !gate;
    digitalWrite(GATE_PIN,gate);
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
    pinMode(PC0,OUTPUT);
    TCC0_HARDABS.SetTimerDivider(256); //Yields 32MHz / 256 / 256 = 1953 Hz per OF
    
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(GATE_PIN, OUTPUT);
    
    
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
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,1,1);
    TCC0_HARDABS.CallMeBackInNOverflows(toggleTrigger,2,0);
    uint8_t val= 128;
    while(1)
    {
        TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelA,val);
        val+=16;
        _delay_ms(100);
       // digitalWrite(PD5, HIGH);
       // digitalWrite(PD4, LOW);
        TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelA,val);
        val+=16;
        _delay_ms(100);
      //  digitalWrite(PD5, LOW);
      //  digitalWrite(PD4, HIGH);
        
    }
    return 0;
}