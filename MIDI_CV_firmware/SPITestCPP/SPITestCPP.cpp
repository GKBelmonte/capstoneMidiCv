/*
 * SPITest.c
 *
 * Created: 17/03/2014 4:17:25 PM
 *  Author: zhadowoflight
 */ 
#include "..\libs\SPI.h"
#include "..\libs\HardAbs.h"
#include <avr/io.h>
#include "..\libs\Timers.h"
#define F_CPU 32000000
#include <util/delay.h>
#define TEST() (PORTC.DIR |= 0b10000000)
#define DAC1_SELECT PC4
#define DAC2_SELECT PC3

uint8_t gate = 0;
void toggleGate()
{
    gate = !gate;
    digitalWrite(PD5,gate);
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,20,1);
}

inline void sendToNoteCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
    command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    //SPILib.SetDataMode(SPI_MODE_0_gc);
    digitalWrite(DAC1_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
    digitalWrite(DAC2_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC2_SELECT,HIGH);
}


inline void sendToVelocityCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x8000;//DAC1B
    command |= 0x1000; //turn on
    command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
   // SPILib.SetDataMode(SPI_MODE_0_gc);
    digitalWrite(DAC1_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
    digitalWrite(DAC2_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC2_SELECT,HIGH);
}

int main(void)
{
    ClockItUp();
    TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
    TCC0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
    TCC0_HARDABS.SetCompareOrCapture(TimerType0::ChannelA, true);
    TCC0_HARDABS.SetWaveformGenerationMode(TimerType0::SingleSlope);
    TCC0_HARDABS.SetCompareChannelValue8bit(TimerType0::ChannelA,127);
    TCC0_HARDABS.SetTimerDivider(256); //Yields 32MHz / 256 / 256 = 1953 Hz per OF
    pinMode(PD5, OUTPUT);
    
    TCC0_HARDABS.CallMeBackInNOverflows(toggleGate,1,1);

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
    
    pinMode(DAC1_SELECT,OUTPUT);
    pinMode(DAC2_SELECT,OUTPUT);
    digitalWrite(DAC2_SELECT,HIGH);
    digitalWrite(DAC1_SELECT,HIGH);
    InitalizeSpiStruct();
    SPILib.SetBitOrder(SPI_MSbFIRST);
    SPILib.SetDataMode(SPI_MODE_0_gc);
    SPILib.Begin();
    sendToVelocityCV(0b0000100000000000);
    uint16_t count = 0;
    while(1)
    {
        
       //sendToVelocityCV(count);
       sendToNoteCV(0);
       count+= 256;
       _delay_ms(1);
      // sendToVelocityCV(count);
       sendToNoteCV(2000);
       count+= 256;
       _delay_ms(1);
       sendToNoteCV(4000);
       count+= 256;
       _delay_ms(1);
       
    }
}