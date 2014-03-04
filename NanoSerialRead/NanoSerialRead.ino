#include "HardwareAbstraction.h"
#include "SPI.h"


void setup()
{
  pinMode(3, OUTPUT);
  /** SPI pins are 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK). */
//  SPI.setBitOrder(MSBFIRST); 
//  SPI.begin();
  Serial.begin(57600);
}

uint8_t stuff= 0;
void loop()
{
  stuff++;  
  if(Serial.available() > 0)
   {
      uint8_t recbyte = Serial.read(); 
      analogWrite(3,recbyte);
      Serial.print("Got: ");
      Serial.println(recbyte);
   }//end switch  
  delay(10);
  if(stuff == 128)
    Serial.println("long time no see");
}

/*
analogWrite(3,stuff++);
  delay(1);
  return;
*/
