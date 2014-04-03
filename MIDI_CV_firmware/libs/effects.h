#ifndef DIGITAL_EFFECTS
#define DIGITAL_EFFECTS
#include "../MIDI_CV_firmware/MIDI_CV_firmware.h"
#include "fastDivide.h"
#include "../libs/Serial.h" //for int to ascii

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

uint8_t arpeggio_period; //0-31, 0 being off
uint8_t arpeggio_state; //0 , 7 or 12
uint8_t arpeggio_stlye; // 1-5-8 , 1-5 or 1-8
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



void clearLCD() 
{ 
    digitalWrite(PC3,LOW);
    SPILib.Transfer((uint8_t)'C');
    SPILib.Transfer((uint8_t)'L');
    digitalWrite(PC3,HIGH);
}

void writeToLCD(const char * str)
{
    //Short SPI on the jumpers
    //jumper alreaddy sorted?
    //8 bit MSB, no info on speed.
    //Data sampled on raise edge of clock. probs SPI mode 0
    //Seding "TRT" text return or CL for clear screen
    //TT to send text
    // 0x00 to end Text transmision.. or 0x0d (which is a new line)
    //CS1 to enable cursor CS0 to disable
    uint8_t c = 0;
    digitalWrite(PC3,LOW);
    SPILib.SetDataMode(SPI_MODE0);
    SPILib.Transfer((uint8_t)'T');
    SPILib.Transfer((uint8_t)'T');
    while(str[c++])
        SPILib.Transfer((uint8_t)str[c]);
    SPILib.Transfer(0);
    digitalWrite(PC3,HIGH);
}

const char * control_display [] =
{
    "IRCon: \0",
    "ArpegPer: \0",
    "ArpegStl: \0",
    "ModulPer: \0",
    "ModulStl: \0",
    "VibPer: \0",
    "VibMag: \0"
};
uint8_t control_state;
uint8_t * control_state_vals [7];
uint8_t control_state_vals_bounds[]=
{
    3,//on-off-cont
    32,
    3, //1-5 etc
    32,
    3,//triangle, + 2 sawtooths
    32,
    16 //mag off also off
};
#define IR_CONTROLLED_STATE 0
#define ARPEGGIO_PERIOD_STATE 1
#define ARPEGGIO_STYLE_STATE 2
#define MODULUS_PERIOD_STATE 3
#define MODULUS_STYLE_STATE 4
#define VIBRATO_PERIOD_STATE 5
#define VIBRATO_MAGNITUDE_STATE 6



void setupControl()
{
    writeToLCD( "Yellow!");
    arpeggio_state=0;
    arpeggio_period =0;
    arpeggio_stlye=0;
    vibrato_magnitude=0;
    vibrato_state=0;
    vibrato_period=0;
    modulus_state=0;
    modulus_type=0;
    modulus_period=0;
    control_state=0;
    control_state_vals[0] = &iRcontrolled;
    control_state_vals[1] = &arpeggio_period;
    control_state_vals[2] = & arpeggio_stlye;
    control_state_vals[3] = &modulus_period;
    control_state_vals[4] = &modulus_type;
    control_state_vals[5] = &vibrato_period;
    control_state_vals[6] = &vibrato_magnitude;
}

const char * EffectEnumMap( uint8_t control_state, uint8_t control_state_val ) 
{
	switch(control_state)
    {
        case 2:
            switch(control_state_val)
            {
                case 0: return "1-5-8\0";
                case 1: return "1-5\0";
                case 2: return "1-8\0";               
            }
            break;
        case 4:
            switch(control_state_val)
            {
                case 0: return "Saw\0";
                case 1: return "InvSaw\0";
                case 2: return "Triangle\0";
            }
            break;
    }
    return "?";
}

void writeLCDNewControlState() 
{
    clearLCD();   
	writeToLCD(control_display[control_state ]) ;
    if( control_state == IR_CONTROLLED_STATE || control_state == ARPEGGIO_PERIOD_STATE  
            || control_state == MODULUS_PERIOD_STATE || control_state == VIBRATO_MAGNITUDE_STATE  || control_state == VIBRATO_PERIOD_STATE   )
    {
        if(*control_state_vals[control_state] == 0)
            writeToLCD("OFF\0");
        else
        {
            char buffer [6] = {0};
            ResultToAscii(*control_state_vals[control_state],buffer);
            writeToLCD(buffer);
        }                    
    }
    else
    {
        writeToLCD(EffectEnumMap (control_state, *control_state_vals[control_state]  ));
    }
    writeToLCD("\n\0");
}

void ToggleActiveEffect( uint8_t  val ) 
{
	switch(control_state)
	{
    	case ARPEGGIO_PERIOD_STATE:
    	    arpeggio_state = 0;
    	    if(val)
    	        TCC0_HARDABS.CallMeBackInNOverflows(Arpeggio , 1, 1);
    	    else
    	        TCC0_HARDABS.CancelCallback(1);
    	    break;
    	case MODULUS_PERIOD_STATE:
    	    arpeggio_state = 0;
    	    if(val)
    	        TCC0_HARDABS.CallMeBackInNOverflows(Modulus , 1, 0);
    	    else
    	        TCC0_HARDABS.CancelCallback(0);
    	    break;
    	case VIBRATO_MAGNITUDE_STATE:
    	    vibrato_state = 0;
    	    if(val)
    	        TCC0_HARDABS.CallMeBackInNOverflows(Vibrato, 1,1);
    	    else
    	        TCC0_HARDABS.CancelCallback(1);
    	    break;
	}
}

void controlCheck()
{
    // to modular subtract add mod size -1 :)
    if( ! (digitalRead(BUTTON_DOWN) && digitalRead(BUTTON_UP) && digitalRead(BUTTON_LEFT) && digitalRead(BUTTON_RIGHT) )  )
    {
        //A button has been pressed
        if(!digitalRead(BUTTON_UP))
        {
            control_state = (control_state +1) % 7;
        }
        else if (!digitalRead(BUTTON_DOWN))
        {
            control_state = (control_state +6) % 7;
        }
        else if (!digitalRead(BUTTON_LEFT))
        {
            uint8_t * val = control_state_vals[control_state];
            uint8_t bounds = control_state_vals_bounds[control_state];
            val[0] = (*val +  bounds- 1 ) %bounds;
            ToggleActiveEffect(*val);

        }
        else
        {
            uint8_t * val = control_state_vals[control_state];
            uint8_t bounds = control_state_vals_bounds[control_state];
            val[0] = (*val +  1 ) %bounds;
            ToggleActiveEffect(*val);
        }
    }
    writeLCDNewControlState();
}
#endif