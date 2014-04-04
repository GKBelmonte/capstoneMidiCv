#ifndef DIGITAL_EFFECTS
#define DIGITAL_EFFECTS
#include "SPI.h"
#include "MIDI_CV_firmware.h"
#include "fastDivide.h"
#include "inttoarr.h" //for int to ascii
#include "Arduino.h"

SoftwareSerial  iamsorry(SOFT_RX,SOFT_TX);
//DigoleSerialDisp  digAHole (11,13,A5);
inline void sendToNoteCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
   // command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    digitalWrite(DAC1_SELECT,LOW);
    SPI.transfer(command>>8);
    SPI.transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}


inline void sendToVelocityCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x8000;//DAC1A
    command |= 0x1000; //turn on
   // command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    digitalWrite(DAC1_SELECT,LOW);
    SPI.transfer(command>>8);
    SPI.transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}

uint8_t arpeggio_period; //0-31, 0 being off
uint8_t arpeggio_state; //0 , 7 or 12
uint8_t arpeggio_stlye; // 1-5-8 , 1-5 or 1-8
void Arpeggio()
{
    CallMeBackInNOverflows(Arpeggio , arpeggio_period << 3, VCO_ISR_ID);

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
       CallMeBackInNOverflows(Modulus , modulus_period << 2, VCA_ISR_ID);
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
    CallMeBackInNOverflows(Vibrato , vibrato_period << 1, VCO_ISR_ID);
    uint8_t mag = FastDivide8( VIBRATO_MAX , 16 - vibrato_magnitude);
    uint16_t eqVoltage = noteMap[note]+ FastDivide8 ( mag , vibrato_state < 16 ? vibrato_state : 16 - (vibrato_state & 0b1111)  )  - (mag >> 1) ;
    sendToNoteCV(eqVoltage);
    vibrato_state = (vibrato_state +1)&0b11111;
}

#define SPITrans

void clearLCD() 
{ 
    //SPI.setClockDivider(SPI_CLOCK_DIV64);
    digitalWrite(LCD_SELECT,LOW);
    digitalWrite(LCD_SELECT,LOW);
    digitalWrite(LCD_SELECT,LOW);
    #ifdef SPITrans
    /*SPI.transfer('C');
    SPI.transfer('L');
    SPI.transfer('T');
    SPI.transfer('P');
    SPI.transfer(0);
    SPI.transfer(0);*/
    iamsorry.print("CLTP");
    iamsorry.write((uint8_t)0);
    iamsorry.write((uint8_t)0);
    delay(1);
    //iamsorry.print("CS");
    //iamsorry.write(1);
    #elif
    shiftOut(11,13,MSBFIRST, 'C');//SPI.transfer('C');
    shiftOut(11,13,MSBFIRST, 'L');    //SPI.transfer('L');
    shiftOut(11,13,MSBFIRST, 'T');//SPI.transfer('T');
    shiftOut(11,13,MSBFIRST, 'P');//SPI.transfer('P');
    shiftOut(11,13,MSBFIRST, 0);//SPI.transfer(0);
    shiftOut(11,13,MSBFIRST, );//SPI.transfer(0);
    #endif
    digitalWrite(LCD_SELECT,HIGH);
   // digAHole.clearScreen();
   // digAHole.setPrintPos(0,0);
    Serial.println();
    
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
    //SPI.setClockDivider(SPI_CLOCK_DIV64);
    digitalWrite(LCD_SELECT,LOW);
    #ifdef SPITrans
    // SPI.setDataMode(SPI_MODE0);
    /*SPI.transfer('T');
    SPI.transfer('T');
    while(str[c++])
    {
      Serial.print('w'); delay(1);
        SPI.transfer(str[c]);
    }
    SPI.transfer(0);*/
    iamsorry.print("TT");
    iamsorry.println(str);
    
    #elif
    //shiftOut(11,13,MSBFIRST, 'T');
    //shiftOut(11,13,MSBFIRST, 'T');
    //while(str[c++])
    //    shiftOut(11,13,MSBFIRST, str[c]);//SPI.transfer(str[c]);
        
    //shiftOut(11,13,MSBFIRST, 0);//SPI.transfer(0);
    #endif//
    
    digitalWrite(LCD_SELECT,HIGH);
    //  digAHole(str);
    Serial.print(str);
}

const char * control_display [] =
{
    "IRCon: ",
    "ArpegPer: ",
    "ArpegStl: ",
    "ModulPer: ",
    "ModulStl: ",
    "VibPer: ",
    "VibMag: "
};
uint8_t control_state;
uint8_t * control_state_vals [7] ;/*= {
     &iRcontrolled,
     &arpeggio_period,
     &arpeggio_stlye,
     &modulus_period,
     &modulus_type,
     &vibrato_period,
     &vibrato_magnitude
};*/
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
    iamsorry = SoftwareSerial (SOFT_RX,SOFT_TX);
    iamsorry.begin(9600);
    iamsorry.println("SB9600");
    clearLCD();
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
    control_state_vals[2] = &arpeggio_stlye;
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
                case 0: return "1-5-8";
                case 1: return "1-5";
                case 2: return "1-8";               
            }
            break;
        case 4:
            switch(control_state_val)
            {
                case 0: return "Saw";
                case 1: return "Triangle";
                case 2: return "InvSaw";
            }
            break;
    }
    return "?";
}

void writeLCDNewControlState() 
{
    //Serial.println("clear");
    delay(1);
    clearLCD();   
    //Serial.print("sts ");
    Serial.println(control_state);
    writeToLCD(control_display[control_state ]) ;
    if( control_state == IR_CONTROLLED_STATE || control_state == ARPEGGIO_PERIOD_STATE  
            || control_state == MODULUS_PERIOD_STATE || control_state == VIBRATO_MAGNITUDE_STATE  || control_state == VIBRATO_PERIOD_STATE   )
    {
      delay(1);
        //Serial.println("if1");
        /*
        if(*control_state_vals[control_state] == 0)
        {
            writeToLCD("OFF");
        }
        else
        {
            //Serial.println("el2");
            char buffer [6] = {0};
            ResultToAscii(*control_state_vals[control_state],buffer);
            //Serial.println(buffer);
            writeToLCD(buffer);
        }*/
        delay(1);        
    }
    else
    {
        //Serial.println("el1");
        writeToLCD(EffectEnumMap (control_state, *control_state_vals[control_state]  ));
        delay(1);
    }
    Serial.println();
    //writeToLCD("\n");
}

void ToggleActiveEffect( uint8_t  val ) 
{
	switch(control_state)
	{
    	case ARPEGGIO_PERIOD_STATE:
    	    arpeggio_state = 0;
    	    if(val)
    	        CallMeBackInNOverflows(Arpeggio , 1, VCO_ISR_ID);
    	    else
    	        CancelCallback(VCO_ISR_ID);
    	    break;
    	case MODULUS_PERIOD_STATE:
    	    arpeggio_state = 0;
    	    if(val)
    	        CallMeBackInNOverflows(Modulus , 1, VCA_ISR_ID);
    	    else
    	        CancelCallback(VCA_ISR_ID);
    	    break;
    	case VIBRATO_MAGNITUDE_STATE:
    	    vibrato_state = 0;
    	    if(val)
    	        CallMeBackInNOverflows(Vibrato, 1,VCO_ISR_ID);
    	    else
    	        CancelCallback(VCO_ISR_ID);
    	    break;
	}
}

void ButtonTriggered(uint8_t up,uint8_t down, uint8_t left, uint8_t right)
{
  
  /*Serial.print(up);
  Serial.print(down);
  Serial.print(left);
  Serial.println(right);*/
  if(!up)
  {
      control_state = (control_state +1) % 7;
  }
  else if (!down)
  {
      control_state = (control_state +6) % 7;
  }
  else if (!left)
  {
      uint8_t * val = control_state_vals[control_state];
      
      uint8_t bounds = control_state_vals_bounds[control_state];
      *val = (*val +  bounds- 1 ) %bounds;
      ToggleActiveEffect(*val);
      

  }
  else
  {
      uint8_t * val = control_state_vals[control_state];
      uint8_t bounds = control_state_vals_bounds[control_state];
      
      *val = (*val +  1 ) %bounds;
      ToggleActiveEffect(*val);
      
  }
  //Serial.println("writtingnew");
  writeLCDNewControlState();
 // Serial.println("Complete");
}

void controlCheck()
{
    uint8_t up =digitalRead(BUTTON_UP), down = digitalRead(BUTTON_DOWN), left
    =digitalRead(BUTTON_LEFT), right =digitalRead(BUTTON_RIGHT);
    // to modular subtract add mod size -1 :)
    if( ! (down && up && left && right )  )
    {
        //A button has been pressed
        ButtonTriggered( up, down,  left,  right);
    }
    
}
#endif
