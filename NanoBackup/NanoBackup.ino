/*
 * MIDI_CV_firmware.cpp
 *
 * Created: 26/03/2014 10:47:50 AM
 *  Author: Gabriel Belmonte
 * License: MIT
 *
 */ 
/*

#define ERROR_LED 8 //TODO: ASSIGN
#define TRIGGER_PIN 2
#define GATE_PIN 3
#define DAC1_SELECT A4
#define DAC2_SELECT A3
#define LCD_SELECT A5


#define IR_SENSOR_PIN_A A7
#define IR_SENSOR_PIN_B P6


#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7
 */ 
#include "SPI.h"
#include "MIDI_CV_firmware.h"
#include "HardwareAbstraction.h"
#include "effects.h"

#define DEBUG

bool readingIrValue = false; //poor mans semaphore

void ReadIRAnalogue()
{
    if(readingIrValue)
        return;
    runningAve[currentIndex] = analogRead(IR_SENSOR_PIN_A); 
    runningAve2[currentIndex] = analogRead(IR_SENSOR_PIN_B);
    currentIndex++;
    currentIndex = currentIndex == POPULATION? 0 : currentIndex;
    CallMeBackInNOverflows(ReadIRAnalogue , 4, IR_ISR_ID);
    // Serial.println("IR");
}
void setup();
void loop();


void setup()
{
  Serial.begin(31250);
  Serial.println("awoke");
  pinMode(13, OUTPUT);
  for(uint8_t ii =0 ; ii < 4;++ii)
  {
    digitalWrite(13,HIGH);
    delay(200);
    digitalWrite(13,LOW);
    delay(400);
  }
  Serial.println("Initializing");
  InitializeTimer(1,0,64,0,true,false); // 4 times faster than before
  //IR sensor
  pinMode(IR_SENSOR_PIN_A, INPUT);
  pinMode(IR_SENSOR_PIN_B, INPUT);
  
  //outputs
  pinMode(9,OUTPUT);
  pinMode(ERROR_LED,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(GATE_PIN, OUTPUT);
  
  //SDI select
  pinMode(DAC1_SELECT, OUTPUT);
  pinMode(DAC2_SELECT, OUTPUT);
  pinMode(LCD_SELECT, OUTPUT);
  
  //inputs
  pinMode(BUTTON_UP,INPUT);
  pinMode(BUTTON_DOWN,INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  
  
  digitalWrite(DAC1_SELECT, HIGH);
  digitalWrite(DAC2_SELECT, HIGH);
  digitalWrite(LCD_SELECT, HIGH);
  
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(TRIGGER_PIN, HIGH);
  digitalWrite(ERROR_LED,HIGH);
  
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  
}

void PulseErrorLED()
{
  digitalWrite(ERROR_LED, LOW);
  Serial.println("ERR");
}
void TriggerOff()
{
  digitalWrite(TRIGGER_PIN, HIGH) ; 
  digitalWrite(ERROR_LED, HIGH);
}

void NotePlayed()
{
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(TRIGGER_PIN, LOW);
  CallMeBackInNOverflows(TriggerOff , 12, TRIGGER_ISR_ID);

  uint16_t eqVoltage = noteMap[note] ;
  sendToNoteCV(eqVoltage);
  
  //Set velocity CV
  sendToVelocityCV( ( ((uint16_t)velocity )<< 5) & 0x0FFF); //   0 <= velocity < 127 -> 0 <= (velocity << 5) < 4064 = 2^12 (so perfect cover) 

  
}
void NotesOff()
{
  sendToVelocityCV(0);
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
    SPI.transfer(command>>8);
    SPI.transfer(command&0xFF);
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
uint8_t IrAverage2()
{
    readingIrValue = true; //lock poor-mans mutex
    uint16_t sum = 0;
    for(uint8_t ii = 0 ; ii < POPULATION ; ++ii)
    {
        sum += runningAve2[ii];
    }
    readingIrValue = false;//unlock poor-mans mutex
    return sum >> 4 ;
}
//#define DEBUG
uint8_t loopcount = 0;




void loop()
{
  //Serial.println("loop");
  //delay(200);

  loopcount++;
  uint8_t irVal = IrAverage();
  if(loopcount %4 == 0 && iRcontrolled )
  {
    uint8_t irVal2 = IrAverage2();
    digitalWrite(GATE_PIN, LOW);
    digitalWrite(TRIGGER_PIN, LOW);
    uint8_t delta = 0;
    uint16_t eqVoltage=0;
    if(iRcontrolled == 2) //continuous mode (inverse too)
        eqVoltage =  ((uint16_t)irVal) <<4; 
    else //discrete
        eqVoltage =  noteMap[ noteFromIr(irVal,0, &delta )];
    
    sendToNoteCV(eqVoltage);
    sendToVelocityCV(((uint16_t)irVal2) <<4);
  }
   if(Serial.available() > 0)
   {
      uint8_t recbyte = Serial.read(); 
      if(recbyte == 254) //Skip active sensing
        return;
      uint8_t event_message = recbyte >> 7;
     // #ifdef DEBUG
      Serial.print('R');
      Serial.println(recbyte);
      Serial.print('#');
      Serial.println(byteNumber);
      Serial.print('T');
      Serial.println(event_message);
     // #endif
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
              Serial.print("M");
              Serial.println(messageType);
              #endif
              if( messageType == NOTE_ON)
              {
                //Note On
                #ifdef DEBUG
                Serial.println("NN");
                #endif
              }
              else if (messageType == NOTE_OFF)
              {
                //Note Off
                NotesOff();
                #ifdef DEBUG
                Serial.print("NO");
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
                Serial.println("IR-E");
                iRcontrolled =  !iRcontrolled;
              }
              else if(messageType == 15)
              {
                //active sensing: N.B. should no longer be reached
                byteNumber = -1;//HACK
              }
              else
              {
                Serial.print("B0-Unrec");
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
           Serial.print("note:");
           Serial.println(note);
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
            Serial.print("B1-Unrec");
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
            Serial.print("Vel");
            Serial.println(velocity);
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
            Serial.println("B2-Unrec");
          }
          byteNumber = 0;
          break;
        }
      }
   }//end switch  

}
