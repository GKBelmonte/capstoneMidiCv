//#include "HardwareAbstraction.h"
#ifndef HARDABS
#define HARDABS
#include "Arduino.h"
#include "SPI.h"

static void (*_callBackID0)() = 0;
static void (*_callBackID1)() = 0;
static void (*_callBackID2)() = 0;
static void (*_callBackID3)() = 0;
static void (*_callBackID4)() = 0;
static void (*_callBackID5)() = 0;
static uint8_t _callBackID0_c = 0;
static uint8_t _callBackID1_c = 0;
static uint8_t _callBackID2_c = 0;
static uint8_t _callBackID3_c = 0;
static uint8_t _callBackID4_c = 0;
static uint8_t _callBackID5_c = 0;

static void (*_callBacks[6])() ={_callBackID0,_callBackID1,_callBackID2,_callBackID3,
                                  _callBackID4,_callBackID5};
static uint8_t _callBackCounts [6] = { _callBackID0_c ,_callBackID1_c, _callBackID2_c,
                                _callBackID3_c, _callBackID4_c, _callBackID5_c};

//#define CancelCallback0() (_callBackID0=0)
//#define CancelCallback0() (_callBackID1=0)
void CancelCallback(uint8_t id)
{
  switch (id)
  {
    case 0:_callBackID0 =0;
      break;
    case 1: _callBackID1 =0;
    break;
    case 2: _callBackID2 = 0;
    break;
    case 3: _callBackID3= 0;
    break;
    case 4: _callBackID4=0;
    break;
    case 5: _callBackID5=0 ; 
    break;
  }
}
void InitializeTimer(uint8_t num, uint8_t initVal, uint16_t prescaler, uint16_t compare, bool OverflowISR_Flag, bool CompareISR_Flag)
{
  switch(num)
  {
    // case 0:
      // TCNT0 = initVal;

      // //Clear prescaler
      // TCCR0 &= 0xF8;
      // if(prescaler==1)
        // TCCR0 |= 0x01;
      // else if(prescaler == 8)
        // TCCR0 |= 0x02;
      // else if(prescaler == 64)
        // TCCR0 |= 0x03;
      // else if(prescaler == 256)
        // TCCR0 |= 0x04;
      // else if(prescaler == 1024)
        // TCCR0 |= 0x05;

      // if(OverflowISR_Flag)
        // TIMSK |= 1 << TOIE0;
      // else
        // TIMSK &= ~(1 << TOIE0);
      // break;
     case 1:
       TCNT1 = initVal;

       TCCR1B &= 0xF8; //Unset the prescaler bits. (i.e. prescaler = 0)
       if(prescaler==1)
         TCCR1B |= 0x01;
       else if(prescaler == 8)
         TCCR1B |= 0x02;
       else if(prescaler == 64)
         TCCR1B |= 0x03;
       else if(prescaler == 256)
         TCCR1B |= 0x04;
       else if(prescaler == 1024)
         TCCR1B |= 0x05;

       OCR1A = compare;

       if(OverflowISR_Flag)
         TIMSK1 |= 1 << TOIE1;
       else
         TIMSK1 &= (1 << TOIE1)^0xFF;

       if(CompareISR_Flag)
         TIMSK1 |= 1 << OCIE1A;
       else
         TIMSK1 &= (1 << OCIE1B) ^ 0xFF;

       break;
    case 2:

      TCNT2 = initVal;

      TCCR2A &= 0xF8; //Unset the prescaler bits. (i.e. prescaler = 0)
      if(prescaler==1)
        TCCR2A |= 0x01;
      else if(prescaler == 8)
        TCCR2A |= 0x02;
      else if(prescaler == 32)
        TCCR2A |= 0x03;
      else if(prescaler == 64)
        TCCR2A |= 0x04;
      else if(prescaler == 128)
        TCCR2A |= 0x05;
      else if(prescaler == 256)
        TCCR2A |= 0x06;
      else if(prescaler == 1024)
        TCCR2A |= 0x07;

      OCR2A = compare;

      if(OverflowISR_Flag)
        TIMSK2 |= 1 << TOIE2;
      else
        TIMSK2 &= (1 << TOIE2)^0xFF;

      if(CompareISR_Flag)
        TIMSK2 |= 1 << OCIE2A;
      else
        TIMSK2 &= (1 << OCIE2A) ^ 0xFF;

    break;
  }
}

ISR(TIMER1_OVF_vect) 
{
  static uint8_t count = 0;
  if(count++ %  32 == 0)
    digitalWrite(3, !digitalRead(3));

   for(uint8_t ii = 0; ii < 6;++ii)
   {
      if(_callBacks[ii] != 0)
      {
        if(--_callBackCounts[ii] == 0)
        {
          void (*t)() = _callBacks[ii] ;
          _callBacks[ii] = 0;
          t();
        }
      }
   }
  /*if(_callBackID0 != 0)
  {
    if(--_callBackID0_c == 0)
    {
      void (*t)() = _callBackID0 ;
      _callBackID0();
      _callBackID0 = t;
    }
  }

  if(_callBackID1 != 0)
  {
    if(--_callBackID1_c == 0)
    {
      void (*t)() = _callBackID1 ;
      _callBackID1();
      _callBackID1 = t;
    }
  }*/
}

/*
* Registers a call-back to be executed in n-overflows of the timer.
* Only two call-backs can be registered at a time, identified by the ID 0 or 1
* Registering a call-back will overwritte any queued call-back.
* Keep the call-back as short as humanly possible.
* For the nano, at 16 MHz and n = 20, the call back executes in 630 ms
*/
void CallMeBackInNOverflows(void(*f)(), uint8_t n,uint8_t ID)
{

  _callBacks[ID]= f;
  _callBackCounts[ID] = n;
}
#endif
