#include "HardwareAbstraction.h"
#include "SPI.h"
#define NOTE_ON 9
#define NOTE_OFF 8
#define PITCH_WHEEL 14
#define CONTROL_CHANGE 11



uint8_t byteNumber = 0;
uint8_t channel = 0;
uint8_t lastMidiMessage = 0;
uint8_t CCType = 0;
uint8_t velocity = 0;

uint8_t note = 0;
uint16_t pitch_wheel = 0x2000;
bool rightChannel = false;
uint8_t messageType;
uint8_t cCVal;


uint16_t runningAve [16];
uint8_t currentIndex = 0;
#define ERROR_LED 4
#define TRIGGER_PIN 5
#define GATE_PIN 6
#define DAC1_SELECT 9 
#define DAC2_SELECT 8 

void ReadIRAnalogue()
{
  runningAve[currentIndex] = analogRead(A7);
  currentIndex++;
  currentIndex = currentIndex == 16? 0 : currentIndex;
  CallMeBackInNOverflows(ReadIRAnalogue , 16, 0);
}

void setup()
{
  Serial.begin(31250);
  InitializeTimer(1,0,1024,0,true,false);
  pinMode(A7, INPUT);
  CallMeBackInNOverflows(ReadIRAnalogue , 16, 0);
  
  
  pinMode(ERROR_LED,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(DAC1_SELECT, OUTPUT);
  pinMode(DAC2_SELECT, OUTPUT);
  digitalWrite(DAC1_SELECT, LOW);
  digitalWrite(DAC2_SELECT, LOW);

  /** SPI pins are 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK). */
  SPI.setBitOrder(MSBFIRST); 
  SPI.begin();
}

void ErrorLEDOff() { }
void PulseErrorLED()
{
  digitalWrite(ERROR_LED, LOW);
}
void TriggerOff()
{
  digitalWrite(TRIGGER_PIN, LOW);
  
  digitalWrite(ERROR_LED, HIGH);
}

void NotePlayed()
{
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(TRIGGER_PIN, HIGH);
  CallMeBackInNOverflows(TriggerOff , 8, 1);
}
void NotesOff()
{
  digitalWrite(GATE_PIN, LOW);
}

void ChangePitch()
{

}


void loop()
{
  
  if(Serial.available() > 0)
   {
      uint8_t recbyte = Serial.read(); 
        uint8_t event_message = recbyte >> 7;
      switch( byteNumber )
      {
        case 0: //MIDI message type and channel 
        {
          uint8_t chan = recbyte & 0x0F; //1st nibble is channel
          if(chan == channel && event_message == 1)
          {
              rightChannel = true;
              messageType = (recbyte >> 4) & 0x0F;
              if( messageType == NOTE_ON)
              {
                //Note On
              }
              else if (messageType == NOTE_OFF)
              {
                //Note Off
                NotesOff();  
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
              else if (messageType == 1100 /*patch change*/
                      ||/*Aftertouch*/ messageType == 1101 )
              {
                //not supported
              }
              else
              {
                PulseErrorLED();
              }
            byteNumber++;    
          }//else not our channel so ignore
          
          break; //End case 0
        }
        case 1: //second MIDI byte
       {   
       
          if(event_message == 1) 
          {
          //There was an error. This ought to
          //be an information signal OR not our channel
          //PulseLEDErrorLight();
           
          }
          else if( messageType == NOTE_ON )
          {
            note = recbyte;
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
            }
          }
          else if(messageType == PITCH_WHEEL)
          {
            //LS(7bits) of pitch wheel
            pitch_wheel = recbyte;
          }
          else if (messageType == 1100 /*patch change*/
                      ||/*Aftertouch*/ messageType == 1101 )
          {
            //not supported (end-of-message)
            byteNumber = 0;
          }
          else
          {
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
            }
            else
            {
              NotePlayed();
            }
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
          }
          byteNumber = 0;
          break;
        }
      }
   }//end switch  

}
