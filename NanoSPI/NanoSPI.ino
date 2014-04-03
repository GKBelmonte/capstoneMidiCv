#include "HardwareAbstraction.h"
#include "SPI.h"
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
#define ERROR_LED 4
#define TRIGGER_PIN 5
#define GATE_PIN 7
#define DAC1_SELECT 6 
#define DAC2_SELECT 8  //unused
bool iRcontrolled = false;

inline void sendToNoteCV (uint16_t volt)
{
    unsigned int command;
    //Set note CV
    command = 0x0000;//DAC1A
    command |= 0x1000; //turn on
    command |= 0x2000;//no gain
    command |= ( volt& 0x0FFF);
    
    digitalWrite(DAC1_SELECT,LOW);
    SPI.transfer(command>>8);
    SPI.transfer(command&0xFF);
    digitalWrite(DAC1_SELECT,HIGH);
}


void clearLCD() 
{ 
    digitalWrite(DAC1_SELECT,LOW);
    SPI.transfer('C');
    SPI.transfer('L');
    SPI.transfer('T');
    SPI.transfer('P');
    SPI.transfer(0);
    SPI.transfer(0);
    digitalWrite(DAC1_SELECT,HIGH);
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
    digitalWrite(DAC1_SELECT,LOW);
    SPI.setDataMode(SPI_MODE1);
    SPI.transfer('T');
    SPI.transfer('T');
    while(str[c++])
        SPI.transfer(str[c]);
    //SPI.transfer(0);
    digitalWrite(DAC1_SELECT,HIGH);
}


void setup()
{
  Serial.begin(31250);
  InitializeTimer(1,0,1024,0,true,false);
  pinMode(A7, INPUT);
  pinMode(3, OUTPUT);//pwm pin
  //SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setBitOrder(MSBFIRST); 
  SPI.begin();
  Serial.begin(31250);
  
  pinMode(DAC1_SELECT, OUTPUT);
  
  digitalWrite(DAC1_SELECT, LOW);
  
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(TRIGGER_PIN, HIGH);
  clearLCD();
  writeToLCD("hello");
}

uint16_t loopcount = 0;

void loop()
{
   loopcount += 64; 
  sendToNoteCV (loopcount);

  delay(1);
  
  sendToNoteCV (~loopcount);

  delay(1);

   if(Serial.available() > 0)
   {
      uint8_t recbyte = Serial.read(); 
      Serial.print(recbyte);
   }//end switch  

}
