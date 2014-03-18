

#ifndef SERIAL_DRIVER
#define SERIAL_DRIVER
#define PARITY_MODE_bm 0b00110000
#define DATA_SIZE_bm 0b00000111

#define SERIAL_COMM_MODE_bm 0b11000000
#define SERIAL_COMM_MODE_bp 6
#define SERIAL_COMM_MODE_ASYNC 0
#define SERIAL_COMM_MODE_SYNC 1
#define SERIAL_COMM_MODE_IR_DA 2
#define SERIAL_COMM_MODE_MASTER_SPI 3

#define SERIAL_STOP_BIT_bm 0b00001000
#define SERIAL_STOP_BIT_bp 3

#define SERIAL_TRANSMIT_INTERRUPT_LEVEL_bm 0b00001100
#define SERIAL_TRANSMIT_INTERRUPT_LEVEL_bp 2
#define SERIAL_RECEIVE_INTERRUPT_LEVEL_bm 0b00110000
#define SERIAL_RECEIVE_INTERRUPT_LEVEL_bp 4
#define SERIAL_DATA_EMPTY_INTERRUPT_LEVEL_bm 0b00000011
#define SERIAL_DATA_EMPTY_INTERRUPT_LEVEL_bp 0

#define SERIAL_BSCALE_bp 4

#define SERIAL_TRANSMIT_ENABLE_bm 0b00001000
#define SERIAL_RECEIVE_ENABLE_bm 0b00010000

enum Parity {
    None,
    Even,
    Odd
};


#include "HardAbs.h"
#include <avr/io.h>
#include <avr/interrupt.h>

    void _setStopBits(uint8_t stopBits);
    void _setDataBits(uint8_t dataBits);
    void _begin(uint16_t baudRate);
    void _setCommunicationMode(uint8_t mode);
    void _setParity(enum Parity stopbits);
    void _setReceiveInterruptLevel(uint8_t level);
    void _setTransmitInterruptLevel(uint8_t level);
    void _setDataRegisterEmptyInterruptLevel(uint8_t level);
    void _setBaudRate(uint16_t baudRate);
    void _sendSingleSync(char  what);
    void _sendSync(char * what);
    uint8_t _avalaible();
    char _read();



struct Serial_struct
{
    void (*SetStopBits)(uint8_t stopBits);
    void (*SetDataBits)(uint8_t dataBits);
    void (*Begin)(uint16_t baudRate);
    void (*SetCommunicationMode)(uint8_t mode);
    void (*SetParity)(enum Parity stopbits);
    void (*SetReceiveInterruptLevel)(uint8_t level);
    void (*SetTransmitInterruptLevel)(uint8_t level);
    void (*SetDataRegisterEmptyInterruptLevel)(uint8_t level);
    void (*SendSingleSync)(char  what);
    void (*SendSync)(char * what);
    uint8_t (*Avalaible) ();
    char (*Read) ();
} Serial;
    char _serial_buffer[64];
    uint8_t _serial_buff_read_pos;//where last user read from buffer
    uint8_t _serial_buff_write_pos;//where last char was placed in buffer
    void InitializeSerialStruct()
    {
        Serial.SetDataBits = _setDataBits;  
        Serial.SetStopBits = _setDataBits;
        Serial.Begin = _begin;
        Serial.SetParity = _setParity; 
        Serial.SetCommunicationMode = _setCommunicationMode;  
        Serial.SetDataRegisterEmptyInterruptLevel = _setDataRegisterEmptyInterruptLevel;
        Serial.SetReceiveInterruptLevel = _setReceiveInterruptLevel;
        Serial.SetTransmitInterruptLevel = _setTransmitInterruptLevel;
        Serial.SendSingleSync = _sendSingleSync;
        Serial.SendSync = _sendSync;
        Serial.Avalaible = _avalaible;
        Serial.Read = _read;
        for(int ii = 0 ; ii < 64 ; ++ii)
        {
            _serial_buffer[ii] = 0;                
        }
        _serial_buff_read_pos=0;
        _serial_buff_write_pos=0;
    }

    void _setStopBits(uint8_t stopBits)
    {
         USARTD0.CTRLC &= ~(SERIAL_STOP_BIT_bm);
         USARTD0.CTRLC &= ( (stopBits -1)<< SERIAL_STOP_BIT_bp) &SERIAL_STOP_BIT_bm;       
    }
    void _setCommunicationMode(uint8_t mode)
    {
        USARTD0.CTRLC &= ~(SERIAL_COMM_MODE_bm);
        USARTD0.CTRLC |= (mode << SERIAL_COMM_MODE_bp) & SERIAL_COMM_MODE_bm;        
    }
    void _setDataBits(uint8_t dataBits)
    {
        USARTD0.CTRLC &= ~(DATA_SIZE_bm);
        switch(dataBits)
        {
            case 5:
                 //done. cleared bits
                break;
            case 6:
                USARTD0.CTRLC |= 1;
                break;
            case 7:
                USARTD0.CTRLC |= 2;
                break;
            case 8:
                USARTD0.CTRLC |= 3;
                break;
            case 9:
                USARTD0.CTRLC |= 7;   
                break;                   
        }
    }
    
    void _setParity(enum Parity stopbits)
    {
        USARTD0.CTRLC &= ~(PARITY_MODE_bm);//clear bits
        switch(stopbits)
        {
            case None:
                //Done. bits cleared
                break;
            case Even:
                USARTD0.CTRLC |= 0b00100000;
                break;
            case Odd:
                USARTD0.CTRLC |= 0b00110000;
                break;
        }
    }
    void _begin(uint16_t baudRate)
    {
        // PD2:22: Serial  RX
        //PD3:23: Serial TX
        digitalWrite(PD2,HIGH);
        pinMode(PD2,INPUT);
        pinMode(PD3,OUTPUT);
        
        Serial.SetDataBits(8);
        Serial.SetParity(None);
        Serial.SetStopBits(1);
        _setBaudRate(baudRate);
        //Enable receieve
        USARTD0.CTRLB |= SERIAL_TRANSMIT_ENABLE_bm;
        //Enable transmit
        USARTD0.CTRLB |= SERIAL_RECEIVE_ENABLE_bm;
        //Enable on receive interrupt enable
        Serial.SetReceiveInterruptLevel(INTERRUPT_LEVEL_1);
    }
    
    uint8_t _avalaible()
    {
        return _serial_buff_write_pos - _serial_buff_read_pos;
    }
    char _read()
    {
        if(_serial_buff_read_pos != _serial_buff_write_pos)
        {
            char res = _serial_buffer[ 0b00111111&(_serial_buff_read_pos)];
            _serial_buff_read_pos++;
            return res;
        }
        else
            return 0;
    }
    ISR(USARTD0_RXC_vect)
    {
        _serial_buffer[++_serial_buff_write_pos] = USARTD0.DATA;
    }
    
    void _sendSingleSync(char  what)
    {
        while(!( 1&(USARTD0.STATUS >> USART_DREIF_bp) ))
            USARTD0.DATA = what;
    }
    void _sendSync(char * what)
    {
        uint8_t pos = 0;
        while(what[pos++] != 0)
        {
            _sendSingleSync(what[pos]);            
        }
    }
    
    void _setBaudRate(uint16_t baudRate)
    {
         /*
        (bps)   CLK2X = 0                   CLK2X = 1
                BSEL    BSCALE Error[%]     BSEL BSCALE  Error [%]
        2400    12      6       0.2         12      7    0.2
        4800    12      5       0.2         12      6    0.2
        9600    12      4       0.2         12      5    0.2
        14.4k   34      2       0.8         34      3    0.8
                138     0       -0.1        138     1   -0.1
        19.2k   12      3       0.2         12      4    0.2
        28.8k   34      1       -0.8        34      2   -0.8
                137     -1      -0.1        138     0   -0.1
        38.4k   12      2       0.2         12      3    0.2
        57.6k   34       0      -0.8        34      1   -0.8
                135     -2      -0.1        137     -1  -0.1
        76.8k   12       1      0.2         12      2    0.2
        115.2k  33      -1      -0.8        34      0   -0.8
                131     -3      -0.1        135     -2  -0.1
        230.4k  31      -2      -0.8        33      -1  -0.8
                123     -4      -0.1        131     -3  -0.1
        460.8k  27      -3      -0.8        31      -2  -0.8
                107     -5      -0.1        123     -4  -0.1
                
                calculation for baud is given by
                Baud= F_CPU /( 2^Bscale * 16 (BSEL+1) )  if BSEL >= 0
                Baud= F_CPU /(16*( 2^Bscale * BSEL )+1 )  o.w.
                
                BSEL = F_CPU / (2^BSCALE *16 * BAUD) - 1
                BSEL = 1/(2^BSCALE) *(F_CPU/(16*BAUD) -1 )  
         */
         USARTD0.BAUDCTRLA = 0;
         USARTD0.BAUDCTRLB = 0;
         switch (baudRate)
         {
            case 2400:
                USARTD0.BAUDCTRLA |= 12;
                USARTD0.BAUDCTRLB |= 6 << SERIAL_BSCALE_bp;
                break;
            case 4800:
                USARTD0.BAUDCTRLA |= 12;
                USARTD0.BAUDCTRLB |= 5 << SERIAL_BSCALE_bp;
                break;
            case 9600:
                USARTD0.BAUDCTRLA |= 12;
                USARTD0.BAUDCTRLB |= 4 << SERIAL_BSCALE_bp;
                break;
            case 31250:
            //bscale is 2
            //bsel is 16
                 USARTD0.BAUDCTRLA |= 15;
                  USARTD0.BAUDCTRLB |= 2 << SERIAL_BSCALE_bp;
                break;
            default:
                //In general.
                // Divide 32000000 by your baud rate THEN by 16.
                // Divide that number by a power P of 2 < 128, such that
                // the result is less than 2^8, called Res
                // log_2(P) = BSCALE
                // Res - 1 = BSEL
                if(baudRate < 62500)
                {
                    
                
                //This case we assume BSCALE = 5, since 32000000/16/32 = 62500 < 2^16
                    USARTD0.BAUDCTRLB |= 5 << SERIAL_BSCALE_bp;
                    uint16_t b_sel_approx = 62500/baudRate -1;
                    USARTD0.BAUDCTRLA |= b_sel_approx;
                    USARTD0.BAUDCTRLB |= 0x0F & b_sel_approx;
                }                
                break;        
         }

    }
    
    
    void _setReceiveInterruptLevel(uint8_t level) 
    { USARTD0.CTRLA &= ~(SERIAL_RECEIVE_INTERRUPT_LEVEL_bm);
      USARTD0.CTRLA |= (level << SERIAL_RECEIVE_INTERRUPT_LEVEL_bp) ;  }
    void _setTransmitInterruptLevel(uint8_t level)
    { USARTD0.CTRLA &= ~(SERIAL_TRANSMIT_INTERRUPT_LEVEL_bm);
      USARTD0.CTRLA |= (level << SERIAL_TRANSMIT_INTERRUPT_LEVEL_bp) ;  }
    void _setDataRegisterEmptyInterruptLevel(uint8_t level)
    { USARTD0.CTRLA &= ~(SERIAL_DATA_EMPTY_INTERRUPT_LEVEL_bm);
      USARTD0.CTRLA |= (level << SERIAL_DATA_EMPTY_INTERRUPT_LEVEL_bp) ;  }
#endif