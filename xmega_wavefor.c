#include <stdio.h>
#include <avr\io.h>
#define F_CPU 32000000UL
#include <util\delay.h>
#include <math.h>

void GenerateArbWave12(int *data, int len, unsigned long int freq);
void SetWaveFreq(unsigned long int freq);
void LoadSineWave(int len);
void Config32MHzClock(void);

volatile int data12[100];
volatile int gWaveNumSamp=50;

int main(void)
{
  int Reading;

  Config32MHzClock();

  CLK.PSCTRL = 0x00; // no division on peripheral clock

  PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PE7_gc;
  PORTE.DIR = (1<<7); // clkout
  TCC0.CTRLA = 4;

// setup DAC output on PORTB:1 as GND reference
  PORTB.DIR |= 0x02;
  PORTB.OUT &= 0xFD;

// setup ADC input on PORTA:0-3 (0=hi, 1=hi, 2=samp, 3=gnd)
// and power PORTA:1 to create voltage divider
  PORTA.DIR = 0xB;
  PORTA.OUT = 0x3;

// setup adc for single ended one shot mode
  ADCA.CTRLA |= 0x1;       // enable adc
  ADCA.CTRLB = 0x4;        // set 8 bit conversion
  ADCA.CH0.CTRL = 0x1;     // single ended
  ADCA.CH0.MUXCTRL = 0x10; // PORTA:2
  ADCA.REFCTRL = 0x20;     // reference is PORTA:0
  ADCA.PRESCALER = 0x5;    // peripheral clk/128

  LoadSineWave(gWaveNumSamp);

  // startup in 10kHz
  GenerateArbWave12(data12,gWaveNumSamp*2,10000);

  while(1)
  {
  // read adc to determine waveform freq
    ADCA.CTRLA |= 0x4;               // start conversion ch0
    while(!ADCA.CH0.INTFLAGS);       // wait for conversion complete flag
    ADCA.CH0.INTFLAGS = 0;           // clear int flags  
	Reading = ADCA.CH0RESL;          // read 8 bit value from POT
    SetWaveFreq((Reading*Reading)+1); // set freq
    _delay_ms(100); 
  };
  
};

void LoadSineWave(int len)
{
int i;

  for(i=0;i<len;i++)
  {
    data12[i]=((sin((2.0/(float)len)*(float)i*M_PI)*0.5 + 0.5)*4095);
  };

};

void SetWaveFreq(unsigned long int freq)
{
    TCD0.PER = F_CPU/freq/gWaveNumSamp/4;
};

void GenerateArbWave12(int *data, int len, unsigned long int freq)
{

EVSYS.CH1MUX = 0xD0;    // CH1 = TCD0 overflow
TCD0.CTRLA = 0x03;      // Prescaler: clk/4
TCD0.PER   = F_CPU/(len/2)/freq/4;        // 31=1MHz,63=500K,127=250K
DACB.CTRLA = 0x05;      // Enable DACB and CH0
DACB.CTRLB = 0x01;  // CH0 auto triggered by an event (CH1)
DACB.CTRLC = 0x08;  // Use AVCC (3.3V), left adjust
DACB.EVCTRL = 0x01; // Event CH1 triggers the DAC Conversion
DACB.TIMCTRL = 0x50;// Minimum 32 CLK between conversions
DMA.CTRL = 0x80;    // Enable, single buffer, round robin
DMA.CH0.ADDRCTRL = 0xD9;// Reload, Increment source
DMA.CH0.TRIGSRC= 0x25;  // DACB CH0 is trigger source
DMA.CH0.TRFCNT = len;   // Buffer is len bytes
DMA.CH0.SRCADDR0  =(((uint32_t)data)>>0*8) & 0xFF;
DMA.CH0.SRCADDR1  =(((uint32_t)data)>>1*8) & 0xFF;
DMA.CH0.SRCADDR2  =(((uint32_t)data)>>2*8) & 0xFF;
DMA.CH0.DESTADDR0 =(((uint32_t)(&DACB.CH0DATA))>>0*8)&0xFF;
DMA.CH0.DESTADDR1 =(((uint32_t)(&DACB.CH0DATA))>>1*8)&0xFF;
DMA.CH0.DESTADDR2 =(((uint32_t)(&DACB.CH0DATA))>>2*8)&0xFF;
DMA.CH0.CTRLA = 0xA5;   // Enable, repeat, 1 byte, single 
};



void Config32MHzClock(void)
{
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  // initialize clock source to be 32MHz internal oscillator (no PLL)
  OSC.CTRL = OSC_RC32MEN_bm; // enable internal 32MHz oscillator
  while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  CLK.CTRL = 0x01; //select sysclock 32MHz osc
//  CCP = CCP_IOREG_gc;
//  CLK.PSCTRL = 0x02; // peripheral clk = sysclk/4, forces sys also /4??
};
