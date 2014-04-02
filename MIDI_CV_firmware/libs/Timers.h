

#ifndef TIMER_LIB
#define TIMER_LIB
#include <avr/io.h>

/* 
XMEGA C [DATASHEET] p.158

CTRLA – Control register A
Bit           7   6   5   4   3   2   1   0
+0x00         –   –   –   –   |CLKSEL[3:0]|
Read/Write    R   R   R   R   R/W R/W R/W R/W
Initial Value 0   0   0   0   0   0   0   0


 Bit 7:4 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
*/

/*
 Bit 3:0 – CLKSEL[3:0]: Clock Select
These bits select the clock source for the timer/counter according to Table 13-3.
CLKSEL=0001 must be set to ensure a correct output from the waveform generator when the hi-res extension is
enabled.
*/
/*
Table 13-3. Clock select options
CLKSEL[3:0]   Group Configuration     Description
0000          OFF                     None              (i.e, timer/counter in OFF state)
0001          DIV1                    Prescaler:  Clk
0010          DIV2                    Prescaler:  Clk/2
0011          DIV4                    Prescaler:  Clk/4
0100          DIV8                    Prescaler:  Clk/8
0101          DIV64                   Prescaler:  Clk/64
0110          DIV256                  Prescaler:  Clk/256
0111          DIV1024                 Prescaler:  Clk/1024
1nnn          EVCHn                   Event channel n, n= [0,...,7]


CTRLB – Control register B

Bit           7     6     5    4    3     2     1     0
+0x01       CCDEN CCCEN CCBEN CCAEN –     |WGMODE[2:0]|
Read/Write  R/W    R/W   R/W  R/W   R    R/W   R/W   R/W
Initial Value 0     0     0    0    0     0     0     0

  Bit 7:4 – CCxEN: Compare or Capture Enable
Setting these bits in the FRQ or PWM waveform generation mode of operation will override the port output register for the
corresponding OCn output pin.
When input capture operation is selected, the CCxEN bits enable the capture operation for the corresponding CC
channel.
  Bit 3 – Reserved
This bit is unused and reserved for future use. For compatibility with future devices, 
always write this bit to zero when this register is written.


 Bit 2:0 – WGMODE[2:0]: Waveform Generation Mode
These bits select the waveform generation mode, and control the counting sequence of the counter, TOP value,
UPDATE condition, interrupt/event condition, and type of waveform that is generated according to Table 13-4.
No waveform generation is performed in the normal mode of operation. For all other modes, the result from the waveform
generator will only be directed to the port pins if the corresponding CCxEN bit has been set to enable this. The port pin
direction must be set as output.



Table 13-4. Timer waveform generation mode.


WGMODE[2:0]     Group Configuration     Mode of Operation   Top     Update      OVFIF/Event
000             NORMAL                       Normal         PER     TOP          TOP
001 FRQ Frequency CCA TOP TOP
010 Reserved - - -
011 SINGLESLOPE Single-slope
PWM PER BOTTOM BOTTOM
100 Reserved - - -
101 DSTOP Dual-slope PWM PER BOTTOM TOP
110 DSBOTH Dual-slope PWM PER BOTTOM TOP and BOTTOM
111 DSBOTTOM Dual-slope PWM PER BOTTOM BOTTOM

  CTRLC – Control register C
Bit 7 6 5 4 3 2 1 0
+0x02 – – – – CMPD CMPC CMPB CMPA
Read/Write R R R R R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0

  Bit 7:4 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
  
  Bit 3:0 – CMPx: Compare Output Value x
These bits allow direct access to the waveform generator's output compare value when the timer/counter is set in the
OFF state. This is used to set or clear the WG output value when the timer/counter is not running.

  CTRLD – Control register D
Bit 7 6 5 4 3 2 1 0
+0x03 EVACT[2:0] EVDLY EVSEL[3:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0

  Bit 7:5 – EVACT[2:0]: Event Action
These bits define the event action the timer will perform on an event according to Table 13-5 on page 160.
The EVSEL setting will decide which event source or sources have control in this case.

Table 13-5. Timer event action selection.
EVACT[2:0] Group Configuration  Event Action
000       OFF                   None
001       CAPT                  Input capture
010       UPDOWN                Externally controlled up/ down count
011       QDEC                  Quadrature decode
100       RESTART               Restart waveform period
101       FRQ                   Frequency capture
110       PW                    Pulse width capture
111       Reserved


Selecting any of the capture event actions changes the behavior of the CCx registers and related status and control bits
to be used for capture. The error status flag (ERRIF) will indicate a buffer overflow in this configuration. See “Event
Action Controlled Operation” on page 150 for further details.

  Bit 4 – EVDLY: Timer Delay Event
When this bit is set, the selected event source is delayed by one peripheral clock cycle. This is intended for 32-bit input
capture operation. Adding the event delay is necessary to compensate for the carry propagation delay when cascading
two counters via the event system.

  Bit 3:0 – EVSEL[3:00]:Timer Event Source Select
These bits select the event channel source for the timer/counter. For the selected event channel to have any effect, the
event action bits (EVACT) must be set according to Table 13-6. When the event action is set to a capture operation, the
selected event channel n will be the event channel source for CC channel A, and event channel (n+1)%8, (n+2)%8, and
(n+3)%8 will be the event channel source for CC channel B, C, and D.
Table 13-6. Timer event source selection
EVSEL[3:0] Group Configuration Event Source
0000 OFF None
0001 – Reserved
0010 – Reserved
0011 – Reserved
0100 – Reserved
0101 – Reserved
0110 – Reserved
0111 – Reserved
1nnn CHn Event channel n, n={0,...,7}
XMEGA C [DATASHEET] 161
8465E–AVR–04/2013
13.12.5 CTRLE – Control register E
? Bit 7:2 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
? Bit 1:0 – BYTEM[1:0]: Byte Mode
These bits select the timer/counter operation mode according to Table 13-7.
Table 13-7. Clock select
13.12.6 INTCTRLA – Interrupt Enable register A
? Bit 7:4 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
? Bit 3:2 – ERRINTLVL[1:0]:Timer Error Interrupt Level
These bits enable the timer error interrupt and select the interrupt level as described in “Interrupts and Programmable
Multilevel Interrupt Controller” on page 114.
? Bit 1:0 – OVFINTLVL[1:0]:Timer Overflow/Underflow Interrupt Level
These bits enable the timer overflow/underflow interrupt and select the interrupt level as described in “Interrupts and
Programmable Multilevel Interrupt Controller” on page 114.
Bit 7 6 5 4 3 2 1 0
+0x04 – – – – – – BYTEM[1:0]
Read/Write R R R R R R R R/W
Initial Value 0 0 0 0 0 0 0 0
BYTEM[1:0] Group Configuration Description
00 NORMAL Timer/counter is set to normal mode (timer/counter type 0)
01 BYTEMODE Upper byte of the counter (CNTH) will be set to zero after each counter
clock cycle
10 SPLITMODE Timer/counter 0 is split into two 8-bit timer/counters (timer/counter type 2)
11 – Reserved
Bit 7 6 5 4 3 2 1 0
+0x06 – – – – ERRINTLVL[1:0] OVFINTLVL[1:0]
Read/Write R R R R R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
XMEGA C [DATASHEET] 162
8465E–AVR–04/2013
13.12.7 INTCTRLB – Interrupt Enable register B
? Bit 7:0 – CCxINTLVL[7:0] - Compare or Capture x Interrupt Level
These bits enable the timer compare or capture interrupt for channel x and select the interrupt level as described in
“Interrupts and Programmable Multilevel Interrupt Controller” on page 114.
13.12.8 CTRLFCLR/CTRLFSET – Control register F Clear/Set
This register is mapped into two I/O memory locations, one for clearing (CTRLxCLR) and one for setting the register bits
(CTRLxSET) when written. Both memory locations will give the same result when read.
The individual status bit can be set by writing a one to its bit location in CTRLxSET, and cleared by writing a one to its bit
location in CTRLxCLR. This allows each bit to be set or cleared without use of a read-modify-write operation on a single
register.
13.12.8.1 CTRLFCLR
13.12.8.2 CTRLFSET
? Bit 7:4 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
? Bit 3:2 – CMD[1:0]: Command
These bits can be used for software control of update, restart, and reset of the timer/counter. The command bits are
always read as zero.
Table 13-8. Command selections
Bit 7 6 5 4 3 2 1 0
+0x07 CCDINTLVL[1:0] CCCINTLVL[1:0] CCBINTLVL[1:0] CCAINTLVL[1:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x08 – – – – CMD[1:0] LUPD DIR
Read/Write R R R R R R R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x09 – – – – CMD[1:0] LUPD DIR
Read/Write R R R R R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
CMD Group Configuration Command Action
00 NONE None
01 UPDATE Force update
10 RESTART Force restart
11 RESET Force hard reset (ignored if T/C is not in OFFstate)
XMEGA C [DATASHEET] 163
8465E–AVR–04/2013
? Bit 1 – LUPD: Lock Update
When this bit is set, no update of the buffered registers is performed, even though an UPDATE condition has occurred.
Locking the update ensures that all buffers, including DTI buffers, are valid before an update is performed.
This bit has no effect when input capture operation is enabled.
? Bit 0 – DIR: Counter Direction
When zero, this bit indicates that the counter is counting up (incrementing). A one indicates that the counter is in the
down-counting (decrementing) state.
Normally this bit is controlled in hardware by the waveform generation mode or by event actions, but this bit can also be
changed from software.
13.12.9 CTRLGCLR/CTRLGSET – Control register G Clear/Set
Refer to “CTRLFCLR/CTRLFSET – Control register F Clear/Set” on page 162 for information on how to access this type
of status register.
? Bit 7:5 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
? Bit 4:1 – CCxBV: Compare or Capture x Buffer Valid
These bits are set when a new value is written to the corresponding CCxBUF register. These bits are automatically
cleared on an UPDATE condition.
Note that when input capture operation is used, this bit is set on a capture event and cleared if the corresponding CCxIF
is cleared.
? Bit 0 – PERBV: Period Buffer Valid
This bit is set when a new value is written to the PERBUF register. This bit is automatically cleared on an UPDATE
condition.
13.12.10INTFLAGS – Interrupt Flag register
? Bit 7:4 – CCxIF: Compare or Capture Channel x Interrupt Flag
The compare or capture interrupt flag (CCxIF) is set on a compare match or on an input capture event on the
corresponding CC channel.
For all modes of operation except for capture, the CCxIF will be set when a compare match occurs between the count
register (CNT) and the corresponding compare register (CCx). The CCxIF is automatically cleared when the
corresponding interrupt vector is executed.
For input capture operation, the CCxIF will be set if the corresponding compare buffer contains valid data (i.e., when
CCxBV is set). The flag will be cleared when the CCx register is read. Executing the interrupt vector in this mode of
operation will not clear the flag.
Bit 7 6 5 4 3 2 1 0
+0x0A/ +0x0B – – – CCDBV CCCBV CCBBV CCABV PERBV
Read/Write R R R R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x0C CCDIF CCCIF CCBIF CCAIF – – ERRIF OVFIF
Read/Write R/W R/W R/W R/W R R R/W R/W
Initial Value 0 0 0 0 0 0 0 0
XMEGA C [DATASHEET] 164
8465E–AVR–04/2013
The flag can also be cleared by writing a one to its bit location.
The CCxIF can be used for requesting a DMA transfer. A DMA read or write access of the corresponding CCx or
CCxBUF will then clear the CCxIF and release the request.
? Bit 3:2 – Reserved
These bits are unused and reserved for future use. For compatibility with future devices, always write these bits to zero
when this register is written.
? Bit 1 – ERRIF: Error Interrupt Flag
This flag is set on multiple occasions, depending on the mode of operation.
In the FRQ or PWM waveform generation mode of operation, ERRIF is set on a fault detect condition from the fault
protection feature in the AWeX extention. For timer/counters which do not have the AWeX extention available, this flag is
never set in FRQ or PWM waveform generation mode.
For capture operation, ERRIF is set if a buffer overflow occurs on any of the CC channels.
For event controlled QDEC operation, ERRIF is set when an incorrect index signal is given.
This flag is automatically cleared when the corresponding interrupt vector is executed. The flag can also be cleared by
writing a one to this location.
? Bit 0 – OVFIF: Overflow/Underflow Interrupt Flag
This flag is set either on a TOP (overflow) or BOTTOM (underflow) condition, depending on the WGMODE setting.
OVFIF is automatically cleared when the corresponding interrupt vector is executed. The flag can also be cleared by
writing a one to its bit location.
OVFIF can also be used for requesting a DMA transfer. A DMA write access of CNT, PER, or PERBUF will then clear the
OVFIF bit.
13.12.11TEMP – Temporary bits for 16-bit Access
The TEMP register is used for single-cycle, 16-bit access to the 16-bit timer/counter registers by the CPU. The DMA
controller has a separate temporary storage register. There is one common TEMP register for all the 16-bit Timer/counter
registers.
For more details, refer to “Accessing 16-bit Registers” on page 13.
13.12.12CNTL – Counter register Low
The CNTH and CNTL register pair represents the 16-bit value, CNT. CNT contains the 16-bit counter value in the
timer/counter. CPU and DMA write access has priority over count, clear, or reload of the counter.
For more details on reading and writing 16-bit registers, refer to “Accessing 16-bit Registers” on page 13.
? Bit 7:0 – CNT[7:0]: Counter low byte
These bits hold the LSB of the 16-bit counter register.
Bit 7 6 5 4 3 2 1 0
+0x0F TEMP[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x20 CNT[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
XMEGA C [DATASHEET] 165
8465E–AVR–04/2013
13.12.13CNTH – Counter register High
? Bit 7:0 – CNT[15:8]: Counter high byte
These bits hold the MSB of the 16-bit counter register.
13.12.14PERL – Period register Low
The PERH and PERL register pair represents the 16-bit value, PER. PER contains the 16-bit TOP value in the
timer/counter.
? Bit 7:0 – PER[7:0]: Period low byte
These bits hold the LSB of the 16-bit period register.
13.12.15PERH – Period register High
? Bit 7:0 – PER[15:8]: Period high byte
These bits hold the MSB of the 16-bit period register.
13.12.16CCxL – Compare or Capture x register Low
The CCxH and CCxL register pair represents the 16-bit value, CCx. These 16-bit register pairs have two functions,
depending of the mode of operation.
For capture operation, these registers constitute the second buffer level and access point for the CPU and DMA.
For compare operation, these registers are continuously compared to the counter value. Normally, the outputs form the
comparators are then used for generating waveforms.
CCx registers are updated with the buffer value from their corresponding CCxBUF register when an UPDATE condition
occurs.
? Bit 7:0 – CCx[7:0]: Compare or Capture x low byte
These bits hold the LSB of the 16-bit compare or capture register.
Bit 7 6 5 4 3 2 1 0
+0x21 CNT[15:8]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x26 PER[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 1 1 1 1 1 1 1 1
Bit 7 6 5 4 3 2 1 0
+0x27 PER[15:8]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 1 1 1 1 1 1 1 1
Bit 7 6 5 4 3 2 1 0
CCx[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
XMEGA C [DATASHEET] 166
8465E–AVR–04/2013
13.12.17CCxH – Compare or Capture x register High
? Bit 7:0 – CCx[15:8]: Compare or Capture x high byte
These bits hold the MSB of the 16-bit compare or capture register.
13.12.18PERBUFL – Timer/Counter Period Buffer Low
The PERBUFH and PERBUFL register pair represents the 16-bit value, PERBUF. This 16-bit register serves as the
buffer for the period register (PER). Accessing this register using the CPU or DMA will affect the PERBUFV flag.
? Bit 7:0 – PERBUF[7:0]: Period Buffer low byte
These bits hold the LSB of the 16-bit period buffer register.
13.12.19PERBUFH – Timer/Counter Period Buffer High
? Bit 7:0 – PERBUF[15:8]: Period Buffer high byte
These bits hold the MSB of the 16-bit period buffer register.
13.12.20CCxBUFL – Compare or Capture x Buffer register Low
The CCxBUFH and CCxBUFL register pair represents the 16-bit value, CCxBUF. These 16-bit registers serve as the
buffer for the associated compare or capture registers (CCx). Accessing any of these registers using the CPU or DMA
will affect the corresponding CCxBV status bit.
? Bit 7:0 – CCxBUF[7:0]: Compare or Capture Buffer low byte
These bits hold the LSB of the 16-bit compare or capture buffer register.
Bit 7 6 5 4 3 2 1 0
CCx[15:8]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
Bit 7 6 5 4 3 2 1 0
+0x36 PERBUF[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 1 1 1 1 1 1 1 1
Bit 7 6 5 4 3 2 1 0
+0x37 PERBUF[15:8]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 1 1 1 1 1 1 1 1
Bit 7 6 5 4 3 2 1 0
CCxBUFx[7:0]
Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
Initial Value 0 0 0 0 0 0 0 0
XMEGA C [DATASHEET] 167
8465E–AVR–04/2013
13.12.21CCxBUFH – Compare or Capture x Buffer register High
? Bit 7:0 – CCxBUF[15:8]: Compare or Capture Buffer high byte
These bits hold the MSB of the 16-bit compare or capture buffer register.
*/
#include "HardAbs.h"
#include <avr/interrupt.h>

class TimerType0 
{
    TC0_struct * inner;
    static const uint8_t CLOCK_PRESCALER_bm = 0b00001111;
    static const uint8_t PRESCALER_1_bm = 1;
    static const uint8_t PRESCALER_2_bm = 2;
    static const uint8_t PRESCALER_4_bm = 3;
    static const uint8_t PRESCALER_8_bm = 4;
    static const uint8_t PRESCALER_64_bm = 5;
    static const uint8_t PRESCALER_256_bm = 6;
    static const uint8_t PRESCALER_1024_bm = 7;
    
    
    public:
    enum ByteMode 
    {
        SixteenBit = 0,
        EightBit = 1,
        Split = 2
    };
    
    enum CompareChannel
    {
        ChannelA = 0,
        ChannelB = 2,
        ChannelC = 4,
        ChannelD = 6
    };
    
    enum WaveformGenerationMode
    {
        Normal = 0, FrequencyGeneration = 1, SingleSlope =3, DualSlopeUpdateTop = 5 , DualSlopeUpdateTopAndBottom = 6 ,DualSlopeUpdateBottom = 7
    };
    
    void SetTimerDivider (uint16_t prescaler)
    {
        inner->CTRLA &= ~ (CLOCK_PRESCALER_bm);//Clear
        switch(prescaler)
        {
            case 1:
                inner->CTRLA |=  PRESCALER_1_bm;
                break;
            case 2:
                inner->CTRLA |= PRESCALER_2_bm;
                break;
            case 4:
                inner->CTRLA |= PRESCALER_4_bm;
                break;
            case 8:
                inner->CTRLA |= PRESCALER_8_bm;
                break;
            case 64:
                inner->CTRLA |= PRESCALER_64_bm;
                break;
            case 256:
                inner->CTRLA |= PRESCALER_256_bm;
                break;
            case 1024:
                inner->CTRLA |= PRESCALER_1024_bm;
                break;
        }
    }

    void SetCompareOrCapture (CompareChannel channel, bool enabled)
    {
        switch(channel)
        {
            case ChannelD:
                if(enabled)
                    inner->CTRLB |= 0b10000000;
                else
                    inner->CTRLB &= ~ 0b10000000; 
                break;
            case ChannelC:
                if(enabled)
                    inner->CTRLB |= 0b01000000;
                else
                    inner->CTRLB &= ~ 0b01000000; 
                break;
            case ChannelB:
                if(enabled)
                    inner->CTRLB |= 0b00100000;
                else
                    inner->CTRLB &= ~ 0b00100000; 
                break;
            case ChannelA:
                if(enabled)
                    inner->CTRLB |= 0b00010000;
                else
                    inner->CTRLB &= ~ 0b00010000; 
                break;           
        }
    }
    
    void SetByteMode(ByteMode mode)
    {
        inner->CTRLE &= ~0b00000011;
        inner->CTRLE |= mode;        
    }
    
    void SetWaveformGenerationMode(WaveformGenerationMode mode)
    {
        inner->CTRLB &=  ~7;
        inner->CTRLB |= mode;       
    }
    
    /*
        Both sets the level AND activates the flag
    */
    void SetOverflowInterruptLevel(uint8_t level)
    {
        inner->INTCTRLA &= ~ (0b00000011);
        inner->INTFLAGS &= ~ (0b00000001);
        switch(level)
        {
            case INTERRUPT_LEVEL_1:
                inner->INTCTRLA |= 0b00000001;
                break;
            case INTERRUPT_LEVEL_2:
                inner->INTCTRLA |= 0b00000010;
                break;
            case INTERRUPT_LEVEL_3:
                inner->INTCTRLA |= 0b00000011;
                break;
        }        
    }

    void SetCompareInterruptLevel(CompareChannel channel, uint8_t level)
    {
        inner->INTCTRLB &= ~(0b11 << channel);
        inner->INTFLAGS &=  ~(1 << (((uint8_t)channel >>2)+4) );
        //channels are 0,2,4,6.
        //-> channel >> 2 = 0 , 1, 2 , 3
        //-> + 4 = 4, 5, 6, 7
        inner->INTCTRLB |= level << channel;
        if(level != INTERRUPT_LEVEL_DISABLED)
            inner->INTFLAGS |=  (1 << (((uint8_t)channel >>1)+4) );
    }
    
    void SetCompareChannelValue(CompareChannel channel, uint16_t val )
    {
        switch(channel)
        {
            case ChannelA:
                inner->CCA = val;
                break;
            case ChannelB:
                inner->CCB = val;
                break;
            case ChannelC:
                inner->CCC = val;
                break;
            case ChannelD:
                inner->CCD = val;
                break;          
        }
    }
    
    void RestartCounter()
    {
        inner->CTRLFSET = 0b10 >> 2;        
    }
    
    void SetPeriod(uint16_t val )
    {
        inner->PER = val;
    }
    
    void CallMeBackInNOverflows(void(*f)(), uint8_t numberOfOF,uint8_t ID)
    {
      if(ID == 0)
      {
        _callBackID0 = f;
        _callBackID0_c = numberOfOF;
      }
      else
      {
        _callBackID1 = f;
        _callBackID1_c = numberOfOF;
      }
    }
    
    void CancelCallback(uint8_t ID)
    {
        if(ID == 0)
        {
            _callBackID0 = 0;
            _callBackID0_c = 0;
        }
        else
        {
            _callBackID1 = 0;
            _callBackID1_c = 0;
        }
    }
    
    void (*_callBackID0)() ;
    void (*_callBackID1)() ;
    uint8_t _callBackID0_c ;
    uint8_t _callBackID1_c ;
    
    TimerType0(TC0_t * address)
    {
        this->inner = address;
        _callBackID0= 0;
        _callBackID0_c = 0;
        _callBackID1 = 0;
        _callBackID1_c = 0;
    }
} 
TCC0_HARDABS(&TCC0), //PC0 10 OC0C
                     //PC1 11 OC0D 
TCD0_HARDABS(&TCD0), 
TCE0_HARDABS(&TCE0);

ISR(TCC0_OVF_vect) 
{
  if(TCC0_HARDABS._callBackID0 != 0)
  {
    if(--TCC0_HARDABS._callBackID0_c == 0)
    {
      TCC0_HARDABS._callBackID0() ; 
      TCC0_HARDABS._callBackID0 = 0;
    }
  }

  if(TCC0_HARDABS._callBackID1 != 0)
  {
    if(--TCC0_HARDABS._callBackID1_c == 0)
    {
      TCC0_HARDABS._callBackID1();
      TCC0_HARDABS._callBackID1 = 0;
    }
  }
}

ISR(TCD0_OVF_vect)
{
    if(TCD0_HARDABS._callBackID0 != 0)
    {
        if(--TCD0_HARDABS._callBackID0_c == 0)
        {
            TCD0_HARDABS._callBackID0() ;
            TCD0_HARDABS._callBackID0 = 0;
        }
    }

    if(TCD0_HARDABS._callBackID1 != 0)
    {
        if(--TCD0_HARDABS._callBackID1_c == 0)
        {
            TCD0_HARDABS._callBackID1();
            TCD0_HARDABS._callBackID1 = 0;
        }
    }
}

struct Timers
{
    void (*GetTimer)();
    
} Timers;


#endif