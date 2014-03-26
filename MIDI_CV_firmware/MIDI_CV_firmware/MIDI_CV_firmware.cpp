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
 
 PA0:40: ADC channel 0
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

/*PIN DEFS*/
#define ERROR_LED 4 //TODO: ASSIGN
#define TRIGGER_PIN PD4
#define GATE_PIN PD5
#define DAC1_SELECT PC3
#define DAC2_SELECT PC4  
#define IR_SENSOR_PIN_A PA0
#define IR_SENSOR_PIN_B PA1

/*MIDI CONSTS*/
#define NOTE_ON 9
#define NOTE_OFF 8
#define PITCH_WHEEL 14
#define CONTROL_CHANGE 11



uint8_t byteNumber = 0;
#define OMNI_ON true
uint8_t channel = 0;
uint8_t lastMidiMessage = 0;
uint8_t CCType = 0;

uint8_t velocity = 0;
uint8_t note = 0;

uint16_t pitch_wheel = 0x2000;
bool rightChannel = false;
uint8_t messageType;
uint8_t cCVal;

const uint8_t POPULATION = 4;
uint16_t runningAve [POPULATION];
uint8_t currentIndex = 0;
bool iRcontrolled = false;


void ReadIRAnalogue()
{
    //runningAve[currentIndex] = analogRead(A7);
    currentIndex++;
    currentIndex = currentIndex == POPULATION? 0 : currentIndex;
    CallMeBackInNOverflows(ReadIRAnalogue , 1, 0);
    // Serial.println("IR");
}
void setup();
void loop();


void setup()
{
  Serial.Begin(31250);
  //InitializeTimer(1,0,1024,0,true,false);
  
  pinMode(A7, INPUT);
  pinMode(3, OUTPUT);//pwm pin
  CallMeBackInNOverflows(ReadIRAnalogue , 4, 0);
  /** SPI pins are 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK). */
  SPI.setBitOrder(MSBFIRST); 
  SPI.begin();
  Serial.begin(31250);
  
  pinMode(ERROR_LED,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(DAC1_SELECT, OUTPUT);
  pinMode(DAC2_SELECT, OUTPUT);
  
  
  digitalWrite(DAC1_SELECT, LOW);
  digitalWrite(DAC2_SELECT, LOW);
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(TRIGGER_PIN, HIGH);
  
  SPILib.SetDataMode(SPI_MODE0);
  
}

void ErrorLEDOff() { }
void PulseErrorLED()
{
  digitalWrite(ERROR_LED, HIGH);
  Serial.println("ERR");
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
  CallMeBackInNOverflows(TriggerOff , 4, 1);
  unsigned int command;
  //Set note CV 
  command = 0x0000;//DAC1A
  command |= 0x1000; //turn on
  //command |= 0x2000;//no gain 
  int eqVoltage =((int)((float)note * 83.333f));//163.3333f));  //TODO: Lookup table
  //Serial.println(eqVoltage);
  command |= ( eqVoltage& 0x0FFF);
  
  digitalWrite(DAC1_SELECT,LOW);
  SPILib.Transfer(command>>8);
  SPILib.Transfer(command&0xFF);
  digitalWrite(DAC1_SELECT,HIGH);
  //Set velocity CV
  /*
  command = 0x8000;//DAC1B
  command |= 0x1000; //turn on
  command |= 0x2000;//no gain 
  command |= ( (velocity << 4) & 0x0FFF);
  SPI.setDataMode(SPI_MODE0);
  digitalWrite(DAC1_SELECT,LOW);
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  digitalWrite(6,HIGH);
  */
}
void NotesOff()
{
  digitalWrite(GATE_PIN, HIGH);
}

void ChangePitch()
{

}

uint8_t IrAverage()
{
  uint16_t sum = 0;
  for(uint8_t ii = 0 ; ii < POPULATION ; ++ii)
  {
    sum += runningAve[ii];
  }
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
    unsigned int command;
    //Set note CV 
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
    //command |= 0x2000;//no gain 
    unsigned int eqVoltage =  ((unsigned int)irVal) << 4; 
    //Serial.println(eqVoltage);
    command |= ( eqVoltage& 0x0FFF);
    
    
    digitalWrite(DAC1_SELECT,LOW);
    SPI_LIB.transfer(command>>8);
    SPI_LIB.transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
  }
    if(Serial.Avalaible() > 0)
   {
      uint8_t recbyte = Serial.read(); 
      if(recbyte == 254) //Skip active sensing
        return;
      uint8_t event_message = recbyte >> 7;
      #ifdef DEBUG
      Serial.SendSingleSync('R');
      Serial.SendSingleSync(recbyte);
      Serial.SendSingleSync('#');
      Serial.SendSingleSync(byteNumber);
      Serial.SendSingleSync('T');
      Serial.SendSingleSync(event_message);
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
              Serial.SendSingleSync(messageType);
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
           Serial.SendSingleSync(note);
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
            Serial.SendSingleSync(velocity);
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
    
    setup();
    while(1)
    {
        loop();
    }
}


