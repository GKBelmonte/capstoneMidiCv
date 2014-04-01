/*
 * AnalogTests.cpp
 *
 * Created: 26/03/2014 2:15:44 PM
 *  Author: zhadowoflight
 */ 


#include <avr/io.h>
#include "../libs/adc.h"
#include "../libs/Timers.h"

int main(void)
{
    ClockItUp();
    TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
    TCC0_HARDABS.SetTimerDivider(256);
    TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelC,128);
    TCC0_HARDABS.SetCompareOrCapture(TimerType0::ChannelC, true);
    ADCLib.BeginDefault();
    
    while(1)
    {
        TCC0_HARDABS.SetCompareChannelValue(TimerType0::ChannelC,ADCLib.analogRead(PA1) );
    }
}