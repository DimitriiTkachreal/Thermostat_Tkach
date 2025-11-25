/*
 * Файл: iostm8s103.h (З папки Cosmic)
 * Стиль: ПРЯМИЙ ДОСТУП (Правильні імена)
 */
#include "iostm8s103.h"

void delay_ms(unsigned int ms) {
    unsigned long i; 
    for (i = 0; i < (ms * 1000UL); i++) {
        // ВИПРАВЛЕННЯ: Cosmic використовує _asm (одне підкреслення)
        _asm("nop");
    }
}

void main(void) {
    // --- Крок 1: Прямий доступ ---
    CLK_CKDIVR = 0x00;
    
    // --- Крок 2: Прямий доступ ---
    PB_DDR |= (1 << 5); 
    PB_CR1 |= (1 << 5); 
    PB_CR2 |= (1 << 5);

    // --- Крок 3: Нескінченний цикл ---
    while (1) {
        PB_ODR ^= (1 << 5);
        delay_ms(500);
    }
}