
#include "SPI.h"


void setup()
{
  pinMode(3, OUTPUT);
  /** SPI pins are 10 (SS), 11 (MOSI), (dntcare)12 (MISO), 13 (SCK). */
  SPI.setBitOrder(MSBFIRST); 
  SPI.begin();
  Serial.begin(31250);
  pinMode(6, OUTPUT);
  digitalWrite(6,HIGH);//6 is CS
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
      unsigned int command;
	
      command = 0x0000;
      command |= /*this->shutdownA*/false ? 0x0000 : 0x1000;
      command |= /*this->gainA*/true ? 0x0000 : 0x2000;
      command |= ((recbyte<<4) & 0x0FFF);
      SPI.setDataMode(SPI_MODE0);
      digitalWrite(6,LOW);
      SPI.transfer(command>>8);
      SPI.transfer(command&0xFF);
      digitalWrite(6,HIGH);
   }//end switch  
  delay(10);
  if(stuff == 128)
    Serial.println("Rec");
}

/*
analogWrite(3,stuff++);
  delay(1);
  return;
*/
