/*
 тестова блимавка
 */
#include "iostm8s103.h"

void delay_ms(unsigned int ms) {
    unsigned long i; 
    for (i = 0; i < (ms * 1000UL); i++) {
        __asm__("nop");
    }
}

void main(void) {
    CLK_CKDIVR = 0x00;
    
  
    PB_DDR |= (1 << 5); 
    PB_CR1 |= (1 << 5); 
    PB_CR2 |= (1 << 5);


    while (1) {
        PB_ODR ^= (1 << 5);
        delay_ms(500);
    }
}