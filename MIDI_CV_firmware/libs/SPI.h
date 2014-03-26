

#ifndef SPI_LIB
#define SPI_LIB
#include <avr/io.h>
//#include <avr/iox32c4.h>

#define SPI_CLOCK_DIV4 (0x00 )
#define SPI_CLOCK_DIV16 (0x01 )
#define SPI_CLOCK_DIV64 (0x02 )
#define SPI_CLOCK_DIV128 (0x03 )
#define SPI_CLOCK_DIV2 (0x80|(0x00 ))
#define SPI_CLOCK_DIV8 (0x80|(0x01 ))
#define SPI_CLOCK_DIV32 (0x80|(0x02 ))
//#define SPI_CLOCK_DIV64 (0x80|(0x03 ))

#define SPI_MASTER 0b00010000

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR

#define SPI_MSbFIRST 0
#define SPI_LSbFIRST 1

#define SPI_ORDER_MASK 0b00100000


/*
    SPI pins for AtXmega32C4
    
    PC5/15/MOSI
    PC6/16/MISO
    PC7/17/SCK
*/
/** 
 Bit 7 – CLK2X: Clock Double
    When this bit is set, the SPI speed (SCK frequency) will be doubled in master mode (see Table 20-3 on page 251).
 Bit 6 – ENABLE: Enable
    Setting this bit enables the SPI module. This bit must be set to enable any SPI operations.
 Bit 5 – DORD: Data Order
    DORD decides the data order when a byte is shifted out from the DATA register. When DORD is written to one, the leastsignificant
    bit (lsb) of the data byte is transmitted first, and when DORD is written to zero, the most-significant bit (msb) of
    the data byte is transmitted first.
 Bit 4 – MASTER: Master Select
    This bit selects master mode when written to one, and slave mode when written to zero. If SS is configured as an input
    and driven low while master mode is set, master mode will be cleared.
 Bit 3:2 – MODE[1:0]: Transfer Mode
    These bits select the transfer mode. The four combinations of SCK phase and polarity with respect to the serial data are
    shown in Table 20-2. These bits decide whether the first edge of a clock cycle (leading edge) is rising or falling, and
    whether data setup and sample occur on the leading or trailing edge.

    When the leading edge is rising, the SCK signal is low when idle, and when the leading edge is falling, the SCK signal is
    high when idle.

    MODE[1:0]   Group Configuration Leading Edge    Trailing Edge
    00          0       Rising,     sample Falling, setup
    01          1       Rising,     setup Falling,  sample
    10          2       Falling,    sample Rising,  setup
    11          3       Falling,    setup Rising,   sample
 Bits 1:0 – PRESCALER[1:0]: Clock Prescaler
    These two bits control the SPI clock rate configured in master mode. These bits have no effect in slave mode. The
    relationship between SCK and the peripheral clock frequency ( clkPER) is shown in Table 20-3.

    CLK2X PRESCALER[1:0]    SCK Frequency
    0       00              ClkPER/4
    0       01              ClkPER/16
    0       10              ClkPER/64
    0       11              ClkPER/128
    1       00              ClkPER/2
    1       01              ClkPER/8
    1       10              ClkPER/32
    1       11              ClkPER/64
*/


void _begin();
inline uint8_t _transfer(uint8_t);
void _setBitOrder(uint8_t);
void _end();
void _setDataMode(uint8_t mode);
void _setClockDivisor(uint8_t divisor);
struct _SPIStruct
{
    void (*Begin)();
    void (*End)();
    uint8_t (*Transfer)(uint8_t t_byte);
    void (*SetBitOrder)(uint8_t order );
    void (*SetDataMode)(uint8_t mode);
} SPILib;

void InitalizeSpiStruct()
{
    SPILib.Begin = _begin;
    SPILib.End = _end;
    SPILib.SetBitOrder = _setBitOrder;
    SPILib.Transfer = _transfer;
    SPILib.SetDataMode = _setDataMode;
}

void _begin()
{
    //MOSI and SCK  and SS as output
    PORTC.DIR |= 0b10110000; 
    PORTC.PIN4CTRL |= PORT_OPC_WIREDANDPULL_gc;
    SPIC.DATA = 0;
    _setClockDivisor(16);//Sets the bps at 2MHz
    SPIC.CTRL |= SPI_MASTER; //Master
    SPIC.CTRL |= SPI_ENABLE_bm; //Start SPI
}
uint8_t _transfer(uint8_t trans)
{
    SPIC.DATA = trans;
    while(~ (SPIC.STATUS & SPI_IF_bm ) );
    uint8_t res = SPIC.DATA;
    return res;
}
void _setBitOrder( uint8_t order)
{
    if(order == SPI_MSbFIRST)
        SPIC.CTRL |= SPI_ORDER_MASK; //LSbfirst
    else
        SPIC.CTRL &= ~SPI_ORDER_MASK; //MSbfirst
}


void _end(){SPIC.CTRL &= ~ SPI_ENABLE_bm;}
void _setDataMode(uint8_t   mode)
{
    SPIC.CTRL &= ~SPI_MODE_MASK;//clear the bits
    SPIC.CTRL |= mode;
}

void _setClockDivisor(uint8_t divisor)
{
    SPIC.CTRL &= ~SPI_CLOCK_MASK; //Clear
    switch (divisor)
    {
        case 2:
            SPIC.CTRL |= SPI_CLOCK_DIV2;
            break;
        case 4:
            SPIC.CTRL |= SPI_CLOCK_DIV4;
            break;
        case 8:
            SPIC.CTRL |= SPI_CLOCK_DIV8;
            break;
        case 16:
            SPIC.CTRL |= SPI_CLOCK_DIV16;
            break;
        case 32:
            SPIC.CTRL |= SPI_CLOCK_DIV32;
            break;
        case 64:
            SPIC.CTRL |= SPI_CLOCK_DIV64;
            break;
    }
}

#endif