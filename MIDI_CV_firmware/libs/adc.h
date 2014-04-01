/*
 CTRLA – Control register A
    Bit 7:3 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
    Bit 2 – CH0START: Channel Start Single Conversion
Setting this bit will start an ADC conversion. Bit is cleared by hardware when the conversion has started. Writing this bit
is equivalent to writing the START bits inside the ADC channel register.
    Bit 1 – FLUSH: Pipeline Flush
Setting this bit will flush the ADC. When this is done, the ADC clock is restarted on the next peripheral clock edge, and
the conversion in progress is aborted and lost.
After the flush and the ADC clock restart, the ADC will resume where it left off; i.e., if any conversions were pending,
these will enter the ADC and complete.
    Bit 0 – ENABLE: Enable
Setting this bit enables the ADC.
*/

/*

CTRLB – ADC Control register B
    Bit 7 – Reserved
This bit is unused and reserved for future use. For compatibility with future devices, always write this bit to zero when this
register is written.
    Bit 6:5 – CURRLIMIT[1:0]: Current Limitation
These bits can be used to limit the current consumption of the ADC by reducing the maximum ADC sample rate. The
available settings are shown in Table 25-1. The indicated current limitations are nominal values. Refer to the device
datasheet for actual current limitation for each setting.

Table 25-1. ADC current limitations.

CURRLIMIT[1:0]      Group Configuration      Description                                            xMega typ Iadc
    00              NO                          No limit                                            1.2mA@16ksps
    01              LOW                         Low current limit, max. sampling rate 225kSPS       1mA@16ksps
    10              MED                         Medium current limit, max. sampling rate 150kSPS    0.9mA@16ksps
    11              HIGH                        High current limit, max. sampling rate 75kSPS       0.8mA@16ksps


CURRLIMIT[1:0]      xMega typ Iadc          xMega typ Iadc
    00              ?                       (all)3.1mA@300ksps
    01              1.7mA   @75ksps
    10              ?
    11              ?
    
    
    Bit 4 – CONVMODE: Conversion Mode
This bit controls whether the ADC will work in signed or unsigned mode. By default, this bit is cleared and the ADC is
configured for unsigned mode. When this bit is set, the ADC is configured for signed mode.
    Bit 3 – FREERUN: Free Running Mode
This bit controls the free running mode for the ADC. Once a conversion is finished, the next input will be sampled and
converted.
    Bit 2:1 : Conversion Result Resolution
These bits define whether the ADC completes the conversion at 12- or 8-bit result resolution. They also define whether
the 12-bit result is left or right adjusted within the 16-bit result registers. See Table 25-2 for possible settings.

Table 25-2. ADC conversion result resolution.
RESOLUTION[1:0]     Group Configuration     Description
00                  12BIT                   12-bit result, right adjusted
01                  –                       Reserved
10                  8BIT                    8-bit result, right adjusted
11                  LEFT12BIT               12-bit result, left adjusted
    Bit 0 – Reserved
This bit is unused and reserved for future use. For compatibility with future devices, always write this bit to zero when this
register is written.
*/


/*
    REFCTRL – Reference Control register
    Bit 7 – Reserved
This bit is unused and reserved for future use. For compatibility with future devices, always write this bit to zero when this
register is written.
    Bits 6:4 – REFSEL[2:0]: Reference Selection
These bits selects the reference for the ADC according to Table 25-3.

REFSEL[2:0] Group Configuration     Description
000         INT1V                   10/11 of bandgap (1.0V)
001         INTVCC                  VCC/1.6
010         AREFA                   External reference from AREF pin on PORT A
011         AREFB                   External reference from AREF pin on PORT B
100         INTVCC2                 VCC/2
101         -                       Reserved
111         -                       Reserved

    Bit 3:2 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
    Bit 1 – BANDGAP: Bandgap Enable
Setting this bit enables the bandgap for ADC measurement. Note that if any other functions are already using the
bandgap, this bit does not need to be set when the internal 1.00V reference is used for another ADC or if the brownout
detector is enabled.
    Bit 0 – TEMPREF: Temperature Reference Enable
Setting this bit enables the temperature sensor for ADC measurement.
*/


/*
PRESCALER – Clock Prescaler register
    Bit 7:3 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
    Bit 2:0 – PRESCALER[2:0]: Prescaler Configuration
These bits define the ADC clock relative to the peripheral clock according to Table 25-6.
Table 25-6. ADC prescaler settings.
PRESCALER[2:0] Group Configuration  Peripheral Clock Division Factor
000             DIV4                    4
001             DIV8                    8
010             DIV16                   16
011             DIV32                   32
100             DIV64                   64
101             DIV128                  128
110             DIV256                  256
111             DIV512                  512
*/

/*
INTFLAGS – Interrupt Flag register
    Bit 7:1 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
    Bit 0 – CH0IF: Interrupt Flags
This flag is set when the ADC conversion is complete. If the ADC is configured for compare mode, the interrupt flag will
be set if the compare condition is met. CH0IF is automatically cleared when the ADC interrupt vector is executed. The
flag can also be cleared by writing a one to its bit location.

*/

#ifndef ADC_LIB
#define ADC_LIB
#include "HardAbs.h"
#include <avr/io.h>

class ADCLib_t
{
    static const uint8_t CURRENT_LIMIT_bm = 0b01100000;
    static const uint8_t CURRENT_LIMIT_bp = 5;

    static const uint8_t SIGN_MODE_bm = 0b00010000;
    static const uint8_t SIGN_MODE_bp = 4;
    
    static const uint8_t RUN_MODE_bm = 0b00001000;
    static const uint8_t RUN_MODE_bp = 3;
    
    static const uint8_t CONVERSION_RESOLUTION_bm = 0b00000110;
    static const uint8_t CONVERSION_RESOLUTION_bp = 1;

    static const uint8_t ANALOG_REFERENCE_bm = 0b01110000;
    static const uint8_t ANALOG_REFERENCE_bp = 4;
    
    static const uint8_t PRESCALER_bm  = 3; //pos =0
    
    public:
        enum CurrentLimitation
        {
            None=0,Low=1,Medium=2,High=3        
        };
        enum SignedMode
        {
            Unsigned=0,Signed=1        
        };
        enum ConversionResolutionMode
        {
            C_12BIT=0,Reserved=1 , C_8BIT  = 2, C_LEFT12BIT =3    
        };
        enum RunMode
        {
            Free=0,NotFree=1        
        };
        enum AnalogReferenceSource
        {
            Voltage1p0 =0,VoltageVccDiv1p6=1 , AREFA = 2, AREFB =3,
            VoltageVccDiv_2 = 4
        };
        enum ClockPrescaler
        {
            Div4 =0,Div8=1 , Div16 =2,
            Div32= 3, Div64 = 4, Div128 = 5 ,
            Div256 = 6, Div512 = 7
        };
        void Begin()
        {
            ADCA.CTRLA |= 0b00000101; //ch0 start and enable

        }
        
        void BeginDefault()
        {
            ADCA.CTRLB |= CURRENT_LIMIT_bm; //High current limitation
            ADCA.CTRLB |= Free <<RUN_MODE_bp; //free run mode
            SetConversionMode(C_8BIT);
            SetVoltageRef(AREFA);
            SetClockPrescaler(Div256);//32MHz/512 = 62KHz. It takes some 8 cycles to settle -> 4KHz
            //This allows for High current limitation 
            
            ADCA.CTRLA |= 0b00000101; //ch0 start and enable
            
        }
        
        void SetCurrentLimitation(CurrentLimitation lim)
        {
            ADCA.CTRLB &= ~(CURRENT_LIMIT_bm)  ;
            ADCA.CTRLB |= lim << CURRENT_LIMIT_bp;       
        }
        
        void SetSignedMode(SignedMode mode)
        {
            ADCA.CTRLB &= ~(SIGN_MODE_bm);
            ADCA.CTRLB |= mode << SIGN_MODE_bp;          
        }
        
        void SetRunMode(RunMode mode)
        {
            ADCA.CTRLB &= ~(RUN_MODE_bm);
            ADCA.CTRLB |= mode << RUN_MODE_bp;          
        }
        
        void SetConversionMode(ConversionResolutionMode mode)
        {
            ADCA.CTRLB &= ~(CONVERSION_RESOLUTION_bm);
            ADCA.CTRLB |= mode << CONVERSION_RESOLUTION_bp;          
        }
    
        void SetVoltageRef(AnalogReferenceSource ref)
        {
            ADCA.REFCTRL &= ~(ANALOG_REFERENCE_bm);
            ADCA.REFCTRL |= ref << ANALOG_REFERENCE_bp;           
        }
        
        void SetClockPrescaler (ClockPrescaler scale)
        {
            ADCA.PRESCALER &= ~(PRESCALER_bm);            
            ADCA.PRESCALER |= scale;//bp = 0
        }
        
        inline uint8_t ConversionComplete ()
        {
            return ADCA.INTFLAGS  &1 ;
        }
        inline void ClearConversion ()
        {
            ADCA.INTFLAGS=1 ;
        }
        
        uint8_t analogRead(uint8_t chan)
        {
            ClearConversion();
            ADCA.CH0.MUXCTRL = 0x07 & chan;
            while(!ADCA.INTFLAGS);
            return ADCA.CH0.RESL;
        }        
} ADCLib;

#endif