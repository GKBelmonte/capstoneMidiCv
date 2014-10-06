/*
 * FastDivideBench.cpp
 *
 * Created: 08/04/2014 4:04:42 PM
 *  Author: zhadowoflight
 */ 


#include <avr/io.h>
#include "../libs/fastDivide.h"

int main(void)
{
    volatile uint8_t ii = 0;
    while(1)
    {
       uint16_t res = FastDivide(230, ii++);
       uint16_t res2 = 230*ii/63;
    }
}