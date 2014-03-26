
#ifndef HARD_ABS
#define HARD_ABS

#define HIGH 1
#define LOW 0

#define INPUT 0
#define OUTPUT 1

#define PortAWrite( _pin_, _level_ ) (PORTA.OUT = _level_ ? (PORTA.OUT | (1 << _pin_ )) : (PORTA.OUT & ~(1 << _pin_))  )
#define PortBWrite( _pin_, _level_ ) (PORTB.OUT = _level_ ? (PORTB.OUT | (1 << _pin_ )) : (PORTB.OUT & ~(1 << _pin_))  )
#define PortCWrite( _pin_, _level_ ) (PORTC.OUT = _level_ ? (PORTC.OUT | (1 << _pin_ )) : (PORTC.OUT & ~(1 << _pin_))  )
#define PortDWrite( _pin_, _level_ ) (PORTD.OUT = _level_ ? (PORTD.OUT | (1 << _pin_ )) : (PORTD.OUT & ~(1 << _pin_))  )

#define PortADir( _pin_, _dir_ ) (PORTA.DIR = _dir_ ? (PORTA.DIR | (1 << _pin_ )) : (PORTA.DIR & ~(1 << _pin_))  )
#define PortBDir( _pin_, _dir_ ) (PORTB.DIR = _dir_ ? (PORTB.DIR | (1 << _pin_ )) : (PORTB.DIR & ~(1 << _pin_))  )
#define PortCDir( _pin_, _dir_ ) (PORTC.DIR = _dir_ ? (PORTC.DIR | (1 << _pin_ )) : (PORTC.DIR & ~(1 << _pin_))  )
#define PortDDir( _pin_, _dir_ ) (PORTD.DIR = _dir_ ? (PORTD.DIR | (1 << _pin_ )) : (PORTD.DIR & ~(1 << _pin_))  )
/*
Pins are defined by the following code
for(var ii = 0; ii < 4 ; ++ii){
    for(var jj =0; jj <8;++jj)
    {
        console.log(
        "#define P"+String.fromCharCode(65+ii)
        + jj +" " +( (ii<<4) + jj) );
    }
}
*/
#define PA0 0
#define PA1 1
#define PA2 2
#define PA3 3
#define PA4 4
#define PA5 5
#define PA6 6
#define PA7 7
#define PB0 16
#define PB1 17
#define PB2 18
#define PB3 19
#define PB4 20
#define PB5 21
#define PB6 22
#define PB7 23
#define PC0 32
#define PC1 33
#define PC2 34
#define PC3 35
#define PC4 36
#define PC5 37
#define PC6 38
#define PC7 39
#define PD0 48
#define PD1 49
#define PD2 50
#define PD3 51
#define PD4 52
#define PD5 53
#define PD6 54
#define PD7 55

#define INTERRUPT_LEVEL_DISABLED 0
#define INTERRUPT_LEVEL_1 1
#define INTERRUPT_LEVEL_2 2
#define INTERRUPT_LEVEL_3 3

void digitalWrite(uint8_t pin, uint8_t high_low)
{
    switch(pin>>4)
    {
        case 0:
            PORTA.OUT = high_low ? (PORTA.OUT | (1 << ((pin & 0x0F) & 0x0F) )) : (PORTA.OUT & ~(1 << (pin & 0x0F)))  ;
            break;
        case 1:
            PORTB.OUT = high_low ? (PORTB.OUT | (1 << ((pin & 0x0F) & 0x0F) )) : (PORTB.OUT & ~(1 << (pin & 0x0F)))  ; 
            break;
        case 2:
            PORTC.OUT = high_low ? (PORTC.OUT | (1 << ((pin & 0x0F) & 0x0F) )) : (PORTC.OUT & ~(1 << (pin & 0x0F)))  ;
            break;
        case 3:
            PORTD.OUT = high_low ? (PORTD.OUT | (1 << ((pin & 0x0F) & 0x0F) )) : (PORTD.OUT & ~(1 << (pin & 0x0F)))  ;
            break;
    }
}

void pinMode(uint8_t pin, uint8_t dir)
{
    switch(pin>>4)
    {
        case 0:
        PORTA.DIR = dir ? (PORTA.DIR | (1 << (pin & 0x0F) )) : (PORTA.DIR & ~(1 << (pin & 0x0F))) ;
        break;
        case 1:
        PORTB.DIR = dir ? (PORTB.DIR | (1 << (pin & 0x0F) )) : (PORTB.DIR & ~(1 << (pin & 0x0F))) ;
        break;
        case 2:
        PORTC.DIR = dir ? (PORTC.DIR | (1 << (pin & 0x0F) )) : (PORTC.DIR & ~(1 << (pin & 0x0F))) ;
        break;
        case 3:
        PORTD.DIR = dir ? (PORTD.DIR | (1 << (pin & 0x0F) )) : (PORTD.DIR & ~(1 << (pin & 0x0F))) ;
        break;
    }
}


uint8_t digitalRead(uint8_t pin)
{
    switch(pin>>4)
    {
        case 0:
            return (PORTA.IN >> (pin & 0x07)) & 0x01  ;
            break;
        case 1:
            return (PORTB.IN >> (pin & 0x07)) & 0x01  ;
            break;
        case 2:
            return (PORTC.IN >> (pin & 0x07)) & 0x01  ;
            break;
        case 3:
            return (PORTD.IN >> (pin & 0x07)) & 0x01  ;
            break;
    }
}


#endif