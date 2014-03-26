

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
    void _sendIntSync(uint16_t what);
    void ResultToAscii(uint16_t res, char * buffer);
    void _sendSync(const char * what);
    uint8_t _avalaible();
    char _read();



struct Serial_struct
{
    void (*SetStopBits)(uint8_t stopBits);
    
    void (*SetDataBits)(uint8_t dataBits);
    
    /*Assumes 8-bit , no parity, 1 stop bits  and consumes PD2 and PD3 as outpus for Serial*/
    void (*Begin)(uint16_t baudRate);
    
    void (*SetCommunicationMode)(uint8_t mode);
   
    void (*SetParity)(enum Parity stopbits);
    
    void (*SetReceiveInterruptLevel)(uint8_t level);
    
    void (*SetTransmitInterruptLevel)(uint8_t level);
    
    void (*SetDataRegisterEmptyInterruptLevel)(uint8_t level);
    
    void (*SendSingleSync)(char  what);
    
    void (*SendSync)(const char * what);
    
    void (*SendIntSync)(uint16_t  what);
    
    uint8_t (*Avalaible) ();
    
    char (*Read) ();
} Serial;



    char _serial_buffer_rx[64];
    uint8_t _serial_buff_read_pos;//where last user read from buffer
    uint8_t _serial_buff_rx_write_pos;//where last char was placed in buffer
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
        Serial.SendIntSync = _sendIntSync;
        for(int ii = 0 ; ii < 64 ; ++ii)
        {
            _serial_buffer_rx[ii] = 0;                
        }
        _serial_buff_read_pos=0;
        _serial_buff_rx_write_pos=0;
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
        //Reset Rx buff
        for(int ii = 0 ; ii < 64 ; ++ii)
        {
            _serial_buffer_rx[ii] = 0;
        }
        _serial_buff_read_pos=0;
        _serial_buff_rx_write_pos=0;
        //Enable receieve
        USARTD0.CTRLB |= SERIAL_TRANSMIT_ENABLE_bm;
        //Enable transmit
        USARTD0.CTRLB |= SERIAL_RECEIVE_ENABLE_bm;
        //Enable on receive interrupt enable
        Serial.SetReceiveInterruptLevel(INTERRUPT_LEVEL_1);
        
    }
    
    uint8_t _avalaible()
    {
        return _serial_buff_rx_write_pos - _serial_buff_read_pos;
    }
    char _read()
    {
        if(_serial_buff_read_pos != _serial_buff_rx_write_pos)
        {
            char res = _serial_buffer_rx[ 0b00111111&(_serial_buff_read_pos)];
            _serial_buff_read_pos++;
            return res;
        }
        else
            return 0;
    }
    
    
    ISR(USARTD0_RXC_vect)
    {
        if(! (USARTD0.STATUS & USART_PERR_bm)) //If parity error, throw away
        {char a = USARTD0.DATA; return;}
        _serial_buffer_rx[(++_serial_buff_rx_write_pos)&0b00111111] = USARTD0.DATA;
        //Since buff is 64 in size, %64 can be done by bit mask 0b0011111111
    }
    char _serial_buffer_tx[64] ={0};
    uint8_t _serial_buff_tx_write_pos =0; //Points to the character that was written to
    uint8_t _serial_buff_tx_read_pos =0; //Points to the character that should be read next
    //TODO: implement async send
    ISR(USARTD0_DRE_vect)
    {
        if (_serial_buff_tx_write_pos == _serial_buff_tx_read_pos) {
            //Done transmitting
            USARTD0.CTRLA &= ~(SERIAL_TRANSMIT_INTERRUPT_LEVEL_bm) ;
            //clear the bits so that the ISR stops executing
        }
        else {
            // There is more data in the output buffer. Send the next byte
            USARTD0.DATA = _serial_buffer_tx[(_serial_buff_tx_read_pos++)&0b00111111];
        }
    }
    
    void _sendSingleSync(char  what)
    {
        while(!( 1&(USARTD0.STATUS >> USART_DREIF_bp) ))
            USARTD0.DATA = what;
    }
    
    void _sendIntSync(uint16_t  what)
    {
        char buffer [6] = {0};
        ResultToAscii(what,buffer);
        _sendSync(buffer);
    }
    
    void _sendSync(const char * what)
    {
        uint8_t pos = 0;
        while(what[pos++] != 0)
        {
            _sendSingleSync(what[pos]);            
        }
    }
    /*
    void _sendSync(const char * what)
    {
        uint8_t pos = 0;
        while(what[pos++] != 0)
        {
            _sendSingleSync(what[pos]);
        }
    }*/
    
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
      
      
/*
	Places the 16 bit number in the string buffer
    ugly implementation, with no loops, but guaranteed speed for sure.
    Keep buffer at least of size 6 (2^16 -1 = 65524 which is 5 characters + null term)
*/
void ResultToAscii(uint16_t res, char * buffer)
{
	/*************************************************************/
    uint8_t charCount = 0;
	if(res >= 50000)
	{
		if(res >=90000)
		{
			buffer[charCount++] = '9';
			res -= 90000;
		}
		else if(res >= 80000)
		{
			buffer[charCount++] = '8';
			res -= 80000;
		}
		else if(res >= 70000)
		{
			buffer[charCount++] = '7';
			res -= 70000;
		}
		else if(res >= 60000)
		{
			buffer[charCount++] = '6';
			res -= 60000;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 50000;
		}			
	}
	else
	{
		if(res >= 40000)
		{
			buffer[charCount++] = '4';
			res -= 40000;
		}
		else if(res >= 30000)
		{
			buffer[charCount++] = '3';
			res -= 30000;
		}
		else if(res >= 20000)
		{
			buffer[charCount++] = '2';
			res -= 20000;
		}
		else if(res >= 10000)
		{
			buffer[charCount++] = '1';
			res -= 10000;
		}	
	}
	/*************************************************************/
	if(res >= 5000)
	{
		if(res >=9000)
		{
			buffer[charCount++] = '9';
			res -= 9000;
		}
		else if(res >= 8000)
		{
			buffer[charCount++] = '8';
			res -= 8000;
		}
		else if(res >= 7000)
		{
			buffer[charCount++] = '7';
			res -= 7000;
		}
		else if(res >= 6000)
		{
			buffer[charCount++] = '6';
			res -= 6000;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 5000;
		}			
	}
	else
	{
		if(res >= 4000)
		{
			buffer[charCount++] = '4';
			res -= 4000;
		}
		else if(res >= 3000)
		{
			buffer[charCount++] = '3';
			res -= 3000;
		}
		else if(res >= 2000)
		{
			buffer[charCount++] = '2';
			res -= 2000;
		}
		else if(res >= 1000)
		{
			buffer[charCount++] = '1';
			res -= 1000;
		}
	}
	/*************************************************************/
	if(res >= 500)
	{
		if(res >=900)
		{
			buffer[charCount++] = '9';
			res -= 900;
		}
		else if(res >= 800)
		{
			buffer[charCount++] = '8';
			res -= 800;
		}
		else if(res >= 700)
		{
			buffer[charCount++] = '7';
			res -= 700;
		}
		else if(res >= 600)
		{
			buffer[charCount++] = '6';
			res -= 600;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 500;
		}			
	}
	else
	{
		if(res >= 400)
		{
			buffer[charCount++] = '4';
			res -= 400;
		}
		else if(res >= 300)
		{
			buffer[charCount++] = '3';
			res -= 300;
		}
		else if(res >= 200)
		{
			buffer[charCount++] = '2';
			res -= 200;
		}
		else if(res >= 100)
		{
			buffer[charCount++] = '1';
			res -= 100;
		}	
	}
	/*************************************************************/
	if(res >= 50)
	{
		if(res >=90)
		{
			buffer[charCount++] = '9';
			res -= 90;
		}
		else if(res >= 80)
		{
			buffer[charCount++] = '8';
			res -= 80;
		}
		else if(res >= 70)
		{
			buffer[charCount++] = '7';
			res -= 70;
		}
		else if(res >= 60)
		{
			buffer[charCount++] = '6';
			res -= 60;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 50;
		}			
	}
	else
	{
		if(res >=40)
		{
			buffer[charCount++] = '4';
			res -= 40;
		}
		else if(res >= 30)
		{
			buffer[charCount++] = '3';
			res -= 30;
		}
		else if(res >= 20)
		{
			buffer[charCount++] = '2';
			res -= 20;
		}
		else if(res >= 10)
		{
			buffer[charCount++] = '1';
			res -= 10;
		}
		else
		{
			buffer[charCount++] = '0';
		}	
	}
	/*************************************************************/
	res += 0x30; //transform the last one to a digit on the fly
	buffer[charCount++] = (char) res;
	buffer[charCount++] = 0; //max here is 5
}
#endif