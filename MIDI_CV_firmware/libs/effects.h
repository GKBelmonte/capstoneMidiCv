#ifndef DIGITAL_EFFECTS
#define DIGITAL_EFFECTS
#include "../MIDI_CV_firmware/MIDI_CV_firmware.h"
#include "fastDivide.h"
uint8_t arpeggio_period; //0-31, 0 being off
uint8_t arpeggio_state; //0 , 7 or 12
uint8_t arpeggio_stlye; // 1-5-8 , 1-5 or 1-8

inline void sendToNoteCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
    command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    
    digitalWrite(DAC1_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}


inline void sendToVelocityCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x8000;//DAC1A
    command |= 0x1000; //turn on
    command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    
    digitalWrite(DAC1_SELECT,LOW);
    SPILib.Transfer(command>>8);
    SPILib.Transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}
void Arpeggio()
{
    TCC0_HARDABS.CallMeBackInNOverflows(Arpeggio , arpeggio_period >> 2, 1);

    uint16_t eqVoltage = noteMap[note + arpeggio_state] ;
    sendToNoteCV(eqVoltage);
    
    switch(arpeggio_stlye)
    {
        case 0: //1-5-8
            arpeggio_state = arpeggio_state == 0 ?  7 :
            ( arpeggio_state == 7 ? 12 : 0);
            break;
        case 1: //1-5
            arpeggio_state = arpeggio_state == 0 ?  7 : 0;
            break;
        case 2: //1-8
            arpeggio_state = arpeggio_state == 0 ?  12 : 0;
            break;
    }
    

    
}

uint8_t modulus_period ; // 0 - 31, 0 being off
uint8_t modulus_state ; // 0 - 15
uint8_t modulus_type ; // 0:Saw, 1:InvertedSaw, 2:Triangle
void Modulus()
{
       TCC0_HARDABS.CallMeBackInNOverflows(Modulus , modulus_period >> 2, 0);
       sendToVelocityCV 
        ( FastDivide16( ((uint16_t)velocity )<< 5 , modulus_state )  & 0x0FFF) ;
        
       switch(modulus_type)
       {
            case 0: //Sawtooth
            modulus_state = (modulus_state + 1) &0b1111;
            break;
            case 2: //Inverted sawtooth
            modulus_state = (modulus_state - 1) &0b1111;
            break;
            case 1: //Triangle
            if(modulus_state == 15)
            {
                modulus_type = 8;
                modulus_state -=1;
            }else modulus_state +=1;
            break;
            
            case 8:
            if(modulus_state == 0)
            {
                modulus_type = 1;
                modulus_state +=1;
            }else modulus_state -=1;
            break;   
       }
}

uint8_t vibrato_period ; //0-31, 0 being off
uint8_t vibrato_state ; 
//0-31    
/*modifier 
if state  < 16 
    (+state) 
else (>= 16) 
    +32-(0b1111 & state) (decrease)

state = (state + 1) &0b11111;
*/
uint8_t vibrato_magnitude ; //0 - 15, with 0 being off
void Vibrato()
{
    TCC0_HARDABS.CallMeBackInNOverflows(Arpeggio , vibrato_period >> 1, 1);

    
    uint8_t mag = FastDivide( VIBRATO_MAX , 16 - vibrato_magnitude);
    uint16_t eqVoltage = noteMap[note]+ FastDivide ( mag , vibrato_state < 16 ? vibrato_state : 16 - (vibrato_state & 0b1111)  )  - (mag >> 1) ;
    sendToNoteCV(eqVoltage);
    vibrato_state = (vibrato_state +1)&0b11111;
}
#endif