/*
 * USARTTest.c
 *
 * Created: 18/03/2014 4:17:43 PM
 *  Author: zhadowoflight
 */ 


#include <avr/io.h>
#include "../libs/Serial.h"
int main(void)
{
    InitializeSerialStruct();
    Serial.Begin(31250);
    sei();
    while(1)
    {
        if(Serial.Avalaible()> 0)
        {
            Serial.SendSync("Received something like: ");
            Serial.SendSingleSync(Serial.Read());
            Serial.SendSingleSync('\n');
        }
    }
}