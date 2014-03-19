/*
 * SPITest.c
 *
 * Created: 17/03/2014 4:17:25 PM
 *  Author: zhadowoflight
 */ 
#include "..\libs\SPI.h"
#include "..\libs\HardAbs.h"
#include <avr/io.h>
#define F_CPU 32
#include <util/delay.h>
#define TEST() (PORTC.DIR |= 0b10000000)
#define DAC1PIN PC4

int main(void)
{
    PortCDir(4,OUTPUT);
    
    InitalizeSpiStruct();
    pinMode(DAC1PIN,OUTPUT);
    digitalWrite(DAC1PIN,HIGH);
    
    SPILib.SetBitOrder(SPI_MSbFIRST);
    SPILib.SetDataMode(SPI_MODE_0_gc);
    SPILib.Begin();
    
    uint8_t count = 0;
    while(1)
    {
       count++;
       digitalWrite(DAC1PIN,LOW);
       SPILib.Transfer(0);
       SPILib.Transfer(count);
       digitalWrite(DAC1PIN,HIGH);
       _delay_ms(250);
       _delay_ms(250);
       _delay_ms(250);
       _delay_ms(250);
       _delay_ms(250);

    }
}