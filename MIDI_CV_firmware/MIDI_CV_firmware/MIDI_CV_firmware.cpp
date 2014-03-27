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

/*PIN DEFS*/
#define ERROR_LED 4 //TODO: ASSIGN
#define TRIGGER_PIN PD4
#define GATE_PIN PD5
#define DAC1_SELECT PC3
#define DAC2_SELECT PC4  
#define IR_SENSOR_PIN_A PA1
#define IR_SENSOR_PIN_B PA2

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
#define THIRTYTHREE_MV_PER_HALF_TONE
extern uint16_t noteMap [];


void ReadIRAnalogue()
{
    //runningAve[currentIndex] = analogRead(A7); //TODO
    currentIndex++;
    currentIndex = currentIndex == POPULATION? 0 : currentIndex;
    TCC0_HARDABS.CallMeBackInNOverflows(ReadIRAnalogue , 1, 0);
    // Serial.println("IR");
}
void setup();
void loop();


void setup()
{
  InitalizeSpiStruct();
  InitializeSerialStruct();
  TCC0_HARDABS.SetByteMode(TimerType0::EightBit);
  TCC0_HARDABS.SetTimerDivider(64); //Yields 32MHz / 256 / 64 = 1953 Hz per OF
  TCC0_HARDABS.SetOverflowInterruptLevel(INTERRUPT_LEVEL_1);
  
  pinMode(IR_SENSOR_PIN_A, INPUT);
 // pinMode(3, OUTPUT);//TODO: Assign/implement
 //TCC0_HARDABS.CallMeBackInNOverflows(ReadIRAnalogue , 4, 0);
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
  TCC0_HARDABS.CallMeBackInNOverflows(TriggerOff , 4, 1);
  unsigned int command;
  //Set note CV 
  command = 0x0000;//DAC1A
  command |= 0x1000; //turn on
  //command |= 0x2000;//no gain 
  uint16_t eqVoltage = noteMap[note] ;
  //Serial.println(eqVoltage);
  command |= ( eqVoltage& 0x0FFF);
  
  digitalWrite(DAC1_SELECT,LOW);
  SPILib.Transfer(command>>8);
  SPILib.Transfer(command&0xFF);
  digitalWrite(DAC1_SELECT,HIGH);
  //Set velocity CV
  command = 0x8000;//DAC1B
  command |= 0x1000; //turn on
  command |= 0x2000;//no gain 
  command |= ( (velocity << 5) & 0x0FFF); //   0 <= velocity < 127 -> 0 <= (velocity << 5) < 4064 = 2^12 (so perfect cover) 
  digitalWrite(DAC1_SELECT,LOW);
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  digitalWrite(DAC1_SELECT,HIGH);
  
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
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
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








#ifdef SIXTEEN_MV_PER_HALF_TONE
uint16_t noteMap[] = 
    {
        /*
        var res = "";
        for(var ii = 0 ; ii < 127; ++ii)
        {
        res += " " + Math.round(ii*16.66) +","+"\n";
        }
        console.log(res);
        */
        0,
        17,
        33,
        50,
        67,
        83,
        100,
        117,
        133,
        150,
        167,
        183,
        200,
        217,
        233,
        250,
        267,
        283,
        300,
        317,
        333,
        350,
        367,
        383,
        400,
        417,
        433,
        450,
        466,
        483,
        500,
        516,
        533,
        550,
        566,
        583,
        600,
        616,
        633,
        650,
        666,
        683,
        700,
        716,
        733,
        750,
        766,
        783,
        800,
        816,
        833,
        850,
        866,
        883,
        900,
        916,
        933,
        950,
        966,
        983,
        1000,
        1016,
        1033,
        1050,
        1066,
        1083,
        1100,
        1116,
        1133,
        1150,
        1166,
        1183,
        1200,
        1216,
        1233,
        1250,
        1266,
        1283,
        1299,
        1316,
        1333,
        1349,
        1366,
        1383,
        1399,
        1416,
        1433,
        1449,
        1466,
        1483,
        1499,
        1516,
        1533,
        1549,
        1566,
        1583,
        1599,
        1616,
        1633,
        1649,
        1666,
        1683,
        1699,
        1716,
        1733,
        1749,
        1766,
        1783,
        1799,
        1816,
        1833,
        1849,
        1866,
        1883,
        1899,
        1916,
        1933,
        1949,
        1966,
        1983,
        1999,
        2016,
        2033,
        2049,
        2066,
        2083,
        2099
    };
#endif 
#ifdef THIRTYTHREE_MV_PER_HALF_TONE
uint16_t noteMap[] =
{
    0,
    33,
    67,
    100,
    133,
    167,
    200,
    233,
    267,
    300,
    333,
    367,
    400,
    433,
    467,
    500,
    533,
    567,
    600,
    633,
    667,
    700,
    733,
    767,
    800,
    833,
    867,
    900,
    933,
    967,
    1000,
    1033,
    1067,
    1100,
    1133,
    1167,
    1200,
    1233,
    1267,
    1300,
    1333,
    1367,
    1400,
    1433,
    1467,
    1500,
    1533,
    1567,
    1600,
    1633,
    1667,
    1700,
    1733,
    1766,
    1800,
    1833,
    1866,
    1900,
    1933,
    1966,
    2000,
    2033,
    2066,
    2100,
    2133,
    2166,
    2200,
    2233,
    2266,
    2300,
    2333,
    2366,
    2400,
    2433,
    2466,
    2500,
    2533,
    2566,
    2600,
    2633,
    2666,
    2700,
    2733,
    2766,
    2800,
    2833,
    2866,
    2900,
    2933,
    2966,
    3000,
    3033,
    3066,
    3100,
    3133,
    3166,
    3200,
    3233,
    3266,
    3300,
    3333,
    3366,
    3400,
    3433,
    3466,
    3500,
    3533,
    3566,
    3600,
    3633,
    3666,
    3700,
    3733,
    3766,
    3800,
    3833,
    3866,
    3900,
    3933,
    3966,
    4000,
    4033,
    4066,
    4100,
    4133,
    4166,
    4200
};
#endif
#ifdef EIGHTYTHREE_MV_PER_HALF_TON
uint16_t noteMap[] =
{
     0,
     83,
     167,
     250,
     333,
     417,
     500,
     583,
     667,
     750,
     833,
     917,
     1000,
     1083,
     1167,
     1250,
     1333,
     1417,
     1500,
     1583,
     1667,
     1750,
     1833,
     1917,
     2000,
     2083,
     2167,
     2250,
     2333,
     2417,
     2500,
     2583,
     2667,
     2750,
     2833,
     2917,
     3000,
     3083,
     3167,
     3250,
     3333,
     3417,
     3500,
     3583,
     3667,
     3750,
     3833,
     3917,
     4000,
     4083,
     4167,
     4250,
     4333,
     4416,
     4500,
     4583,
     4666,
     4750,
     4833,
     4916,
     5000,
     5083,
     5166,
     5250,
     5333,
     5416,
     5500,
     5583,
     5666,
     5750,
     5833,
     5916,
     6000,
     6083,
     6166,
     6250,
     6333,
     6416,
     6500,
     6583,
     6666,
     6750,
     6833,
     6916,
     7000,
     7083,
     7166,
     7250,
     7333,
     7416,
     7500,
     7583,
     7666,
     7750,
     7833,
     7916,
     8000,
     8083,
     8166,
     8250,
     8333,
     8416,
     8500,
     8583,
     8666,
     8750,
     8833,
     8916,
     9000,
     9083,
     9166,
     9250,
     9333,
     9416,
     9500,
     9583,
     9666,
     9750,
     9833,
     9916,
     10000,
     10083,
     10166,
     10250,
     10333,
     10416,
     10500    
}
#endif



int main(void)
{
    
    setup();
    while(1)
    {
        loop();
    }
    return 1;
}


