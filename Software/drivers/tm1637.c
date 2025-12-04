#include "tm1637.h"
#include "gpio_config.h"

// Коротка затримка
void _delay_us(void) { 
    _asm("nop"); _asm("nop"); _asm("nop"); _asm("nop"); 
    _asm("nop"); _asm("nop"); _asm("nop"); _asm("nop");
}

void TM1637_Init(void) {
    // Push-Pull Output для надійності
    DISP_PORT_DDR |= (DISP_CLK_PIN | DISP_DIO_PIN);
    DISP_PORT_CR1 |= (DISP_CLK_PIN | DISP_DIO_PIN); 
    DISP_PORT_CR2 |= (DISP_CLK_PIN | DISP_DIO_PIN);
    
    // Початковий стан - High
    DISP_PORT_ODR |= DISP_CLK_PIN;
    DISP_PORT_ODR |= DISP_DIO_PIN;
}

// --- Службові функції ---
void _start(void) {
    DISP_PORT_ODR |= DISP_DIO_PIN;
    DISP_PORT_ODR |= DISP_CLK_PIN;
    _delay_us();
    DISP_PORT_ODR &= ~DISP_DIO_PIN;
    _delay_us();
    DISP_PORT_ODR &= ~DISP_CLK_PIN;
}

void _stop(void) {
    DISP_PORT_ODR &= ~DISP_CLK_PIN;
    DISP_PORT_ODR &= ~DISP_DIO_PIN;
    _delay_us();
    DISP_PORT_ODR |= DISP_CLK_PIN;
    _delay_us();
    DISP_PORT_ODR |= DISP_DIO_PIN;
}

void _write(unsigned char b) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        DISP_PORT_ODR &= ~DISP_CLK_PIN;
        _delay_us();
        if (b & 0x01) DISP_PORT_ODR |= DISP_DIO_PIN;
        else          DISP_PORT_ODR &= ~DISP_DIO_PIN;
        _delay_us();
        DISP_PORT_ODR |= DISP_CLK_PIN;
        _delay_us();
        b >>= 1;
    }
    
    // ACK
    DISP_PORT_ODR &= ~DISP_CLK_PIN;
    DISP_PORT_ODR |= DISP_DIO_PIN;
    _delay_us();
    DISP_PORT_ODR |= DISP_CLK_PIN;
    _delay_us();
    DISP_PORT_ODR &= ~DISP_CLK_PIN;
}

const unsigned char _seg[] = { 
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f 
};

// Головна функція виводу
void TM1637_Show(unsigned int num) {
    unsigned char i;
    unsigned char d[4];
    
    if(num > 9999) num = 9999;
    d[0]=num/1000; d[1]=(num/100)%10; d[2]=(num/10)%10; d[3]=num%10;

    _start(); _write(0x40); _stop();
    _start(); _write(0xC0);
    for(i=0; i<4; i++) _write(_seg[d[i]]);
    _stop();
    _start(); _write(0x8F); _stop(); 
}