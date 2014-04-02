/*
 * MIDI_CV_firmware.cpp
 *
 * Created: 26/03/2014 10:47:50 AM
 *  Author: Gabriel Belmonte
 * License: MIT
 *
 */ 
/*

 
 PinOut:
 
 PD0:20: USB connected.
 PD2:22: Serial MIDI RX 
 PD3:23: Serial TX debug
 PD4:24: Trigger
 PD5:25: Gate
 PD6:26: D-
 PD7:27: D+
 
 PC3:13: DAC 0 select(active low)
 PC4:14: DAC 1 select(active low)
 PC5:15:MOSI
 PC6:16:MISO
 PC7:17:SCK
 
 PA0:40: ADC channel 0 / AREF
 PA1:41: ADC channel 1    
 PA2:42: ADC channel 2
 
 PWM enabled pins
 PC0:10
 PC1:11
 PC2:12
 */ 

#include <avr/io.h>
#include "..\libs\HardAbs.h"
#include "..\libs\SPI.h"
#include "..\libs\Serial.h"
#include "..\libs\Timers.h"
#include "..\libs\effects.h"
#include "MIDI_CV_firmware.h"

bool readingIrValue = false; //poor mans semaphore

void ReadIRAnalogue()
{
    //runningAve[currentIndex] = analogRead(A7); //TODO
    if(readingIrValue)
        return;
    currentIndex++;
    currentIndex = currentIndex == POPULATION? 0 : currentIndex;
    TCD0_HARDABS.CallMeBackInNOverflows(ReadIRAnalogue , 1, 0);
    // Serial.println("IR");
}
void setup();
void loop();


void setup()
{
  ClockItUp();
  InitalizeSpiStruct();
  InitializeSerialStruct();
  TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
  TCC0_HARDABS.SetWaveformGenerationMode(TimerType0::SingleSlope); //Single slope PWM
  TCC0_HARDABS.SetCompareOrCapture(TimerType0::ChannelC,true);//enable PWM on channel C
  TCC0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
  TCC0_HARDABS.SetTimerDivider(64); //Yields 32MHz / 256 / 64 = 1953 Hz per OF
  pinMode(IR_SENSOR_PIN_A, INPUT);
  pinMode(IR_SENSOR_PIN_B, INPUT);
  pinMode(PC0, OUTPUT);//PC0 is channel C 
  
  TCD0_HARDABS.SetByteMode(TimerType0::EightBit);
  TCD0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
  TCC0_HARDABS.SetTimerDivider(1024); //Yields 32MHz/256 /1024 = 122Hz per OF
  TCD0_HARDABS.CallMeBackInNOverflows(ReadIRAnalogue , 2, 0);
 /** SPI pins are 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK). */

  
  pinMode(ERROR_LED,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(DAC1_SELECT, OUTPUT);
  pinMode(DAC2_SELECT, OUTPUT);
  
  
  digitalWrite(DAC1_SELECT, LOW);
  digitalWrite(DAC2_SELECT, LOW);
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(TRIGGER_PIN, HIGH);
  
  Serial.Begin(31250);
  SPILib.SetBitOrder(SPI_MSbFIRST);
  SPILib.SetDataMode(SPI_MODE0);
  SPILib.Begin();
  //enable all interrupt levels and global
  sei();
  PMIC.CTRL |= 7;
  
}

void ErrorLEDOff() { }
void PulseErrorLED()
{
  digitalWrite(ERROR_LED, HIGH);
  Serial.SendSync("ERR");
}
void TriggerOff()
{
  digitalWrite(TRIGGER_PIN, HIGH);
  
  digitalWrite(ERROR_LED, LOW);
}

void NotePlayed()
{
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(TRIGGER_PIN, LOW);
  TCD0_HARDABS.CallMeBackInNOverflows(TriggerOff , 12, 1);

  uint16_t eqVoltage = noteMap[note] ;
  sendToNoteCV(eqVoltage);
  
  
  //Set velocity CV

  sendToVelocityCV( ( ((uint16_t)velocity )<< 5) & 0x0FFF); //   0 <= velocity < 127 -> 0 <= (velocity << 5) < 4064 = 2^12 (so perfect cover) 

  
}
void NotesOff()
{
  digitalWrite(GATE_PIN, HIGH);
}

void ChangePitch()
{
    //Adjust the pitch depending on our flexibility
    //since its a 14 bit integer shoved in an 8bit, min PITCH_ADJUST is 6
    int8_t effective_shift = ((int16_t)(pitch_wheel - 0x2000)/*center*/) >> PITCH_ADJUST;
    unsigned int command;
    //Set note CV
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
    //command |= 0x2000;//no gain
    uint16_t eqVoltage = noteMap[note] + effective_shift ;
    //Serial.println(eqVoltage);
    command |= ( eqVoltage& 0x0FFF);
    
    digitalWrite(DAC1_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}

uint8_t IrAverage()
{
  readingIrValue = true; //lock poor-mans mutex
  uint16_t sum = 0;
  for(uint8_t ii = 0 ; ii < POPULATION ; ++ii)
  {
    sum += runningAve[ii];
  }
  readingIrValue = false;//unlock poor-mans mutex
  return sum >> 4 ;
}

//#define DEBUG
uint8_t loopcount = 0;




void loop()
{
  loopcount++;
  uint8_t irVal = IrAverage();
  //analogWrite(3,irVal); //TODO
  if(loopcount %4 == 0 && iRcontrolled )
  {
    
    digitalWrite(GATE_PIN, LOW);
    digitalWrite(TRIGGER_PIN, LOW);
    uint8_t delta = 0;
    uint16_t eqVoltage=0;
    if(iRcontrolled == 2) //continious mode (inverse too)
        eqVoltage =  ((uint16_t)irVal) <<4; 
    else //discrete
        eqVoltage =  noteMap[ noteFromIr(irVal,0, &delta )];
    sendToNoteCV(eqVoltage);
  }
   if(Serial.Avalaible() > 0)
   {
      uint8_t recbyte = Serial.Read(); 
      if(recbyte == 254) //Skip active sensing
        return;
      uint8_t event_message = recbyte >> 7;
      #ifdef DEBUG
      Serial.SendSingleSync('R');
      Serial.SendIntSync(recbyte);
      Serial.SendSingleSync('#');
      Serial.SendIntSync(byteNumber);
      Serial.SendSingleSync('T');
      Serial.SendIntSync(event_message);
      #endif
      if(!event_message && messageType == NOTE_ON && byteNumber == 0) //allow after touch
        byteNumber = 1;
      
        
      switch( byteNumber )
      {
        case 0: //MIDI message type and channel 
        {
          uint8_t chan = recbyte & 0x0F; //1st nibble is channel
          if((chan == channel||OMNI_ON) && event_message == 1)
          {
              rightChannel = true;
              messageType = (recbyte >> 4) & 0x0F;
              #ifdef DEBUG
              Serial.SendSync("M");
              Serial.SendIntSync(messageType);
              #endif
              if( messageType == NOTE_ON)
              {
                //Note On
                #ifdef DEBUG
                Serial.SendSync("NN");
                #endif
              }
              else if (messageType == NOTE_OFF)
              {
                //Note Off
                NotesOff();
                #ifdef DEBUG
                Serial.SendSync("NO");
                #endif  
              }
              else if (messageType == PITCH_WHEEL)
              {
              //1110nnnn 	
              //0lllllll
              //0mmmmmmm 	
              //Pitch Wheel Change. 0mmmmmmm 
              //This message is sent to indicate a change in the
              //pitch wheel. The pitch wheel is measured by a fourteen bit value. 
              //Center (no pitch change) is 2000H. Sensitivity is a function of 
              //the transmitter. (llllll) are the least significant 7 bits. (mmmmmm) 
              //are the most significant 7 bits. 
              }
              else if (messageType == 12 /*patch change*/
                      ||/*Aftertouch*/ messageType == 13 )
              {
                //not supported
                Serial.SendSync("IR-E");
                iRcontrolled =  !iRcontrolled;
              }
              else if(messageType == 15)
              {
                //active sensing: N.B. should no longer be reached
                byteNumber = -1;//HACK
              }
              else
              {
                Serial.SendSync("B0-Unrec");
                PulseErrorLED();
              }
              byteNumber++;    
          }//else not our channel so ignore
          
          break; //End case 0
        }
        case 1: //second MIDI byte
       {   
         byteNumber++;
          if(event_message == 1) 
          {
          //There was an error. This ought to
          //be an information signal OR not our channel
          //PulseLEDErrorLight();
           #ifdef DEBUG
           //Serial.println("BEM");
           #endif
          }
          else if( messageType == NOTE_ON )
          {
            note = recbyte;
           #ifdef DEBUG
           Serial.SendSync("note:");
           Serial.SendIntSync(note);
           #endif
          }
          else if(messageType == NOTE_OFF)
          {
            //note = recbyte;
            //can ignore anyways
          }
          else if(messageType == CONTROL_CHANGE)
          {
            CCType = recbyte;
            if (CCType == 0x120 || CCType == 0x123)
            {
              //ADSR goes off (All-sound-off message, all-notes-off message)
              NotesOff();
            }
          }
          else if(messageType == PITCH_WHEEL)
          {
            //LS(7bits) of pitch wheel
            pitch_wheel = recbyte;
          }
          else if (messageType == 12 /*patch change*/
                      ||/*Aftertouch*/ messageType == 13 )
          {
            //not supported (end-of-message)
            byteNumber = 0;
          }
          else
          {
            Serial.SendSync("B1-Unrec");
            byteNumber = 0;
            //Debug error message could be send by uart
            PulseErrorLED();
          }
          
          break;
       }
        case 2: //third MIDI byte
        {  
          if(event_message == 1) 
          {
          //There was an error. This ought to
          //be an information signal OR not our channel
          }
          else if( messageType == NOTE_ON )
          {
            
            velocity = recbyte;
            if(velocity == 0)
            {
              //Turn ADSR off
              NotesOff();
            }
            else
            {
              NotePlayed();
            }
            #ifdef DEBUG
            Serial.SendSync("Vel");
            Serial.SendIntSync(velocity);
            #endif
          }
          else if(messageType == NOTE_OFF)
          {
            //velocity = recbyte;
            //can ignore anyways
          }
          else if(messageType == CONTROL_CHANGE)
          {
            cCVal = recbyte;
            //unused until further notice
          }
          else if(messageType == PITCH_WHEEL)
          {
            //MS(7bits) of pitch wheel
            pitch_wheel = ((uint16_t)recbyte) >> 7;
            ChangePitch();
          }
          else
          {
            //Debug error message could be sent here too.
            PulseErrorLED();
            Serial.SendSync("B2-Unrec");
          }
          byteNumber = 0;
          break;
        }
      }
   }//end switch  

}




int main(void)
{
    setupControl();
    setup();
    while(1)
    {
        controlCheck();
        loop();
    }
    return 1;
}



