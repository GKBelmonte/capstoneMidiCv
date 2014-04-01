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

void toggleTrigger()
{
    static bool state = true;
    digitalWrite(TRIGGER_PIN,!state);
    state = !state;
    TCC0_HARDABS.CallMeBackInNOverflows(toggleTrigger,64,0);
}
void toggleGate()
{
    static bool state = true;
    digitalWrite(GATE_PIN,!state);
    state = !state;
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,64,1); 
}

int main(void)
{
    ClockItUp();
    TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
    TCC0_HARDABS.SetTimerDivider(256); //Yields 32MHz / 256 / 64 = 1953 Hz per OF
    TCC0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
    TCC0_HARDABS.SetCompareOrCapture(TimerType0::ChannelC, true);
    TCC0_HARDABS.SetWaveformGenerationMode(TimerType0::SingleSlope);
    
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(GATE_PIN, OUTPUT);
    
    sei();
    PMIC.CTRL |= 7;
    //TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,8,1);
    //TCC0_HARDABS.CallMeBackInNOverflows(toggleTrigger,16,0);
    uint8_t val= 0;
    while(1)
    {
        TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelC,val);
        val+=16;
        _delay_ms(500);
        digitalWrite(PD5, HIGH);
        digitalWrite(PD4, LOW);
        TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelC,val);
        val+=16;
        _delay_ms(500);
        digitalWrite(PD5, LOW);
        digitalWrite(PD4, HIGH);
        
    }
    return 0;
}