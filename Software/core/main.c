#include "iostm8s103.h"

/* --- НАЛАШТУВАННЯ ПІНІВ --- */
#define CLK_PIN  (1 << 3)  // PC3
#define DIO_PIN  (1 << 4)  // PC4
#define RELAY_PIN (1 << 2) // PD2

// Коди цифр: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
const unsigned char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

/* --- ЗАТРИМКИ --- */
void delay_us(unsigned int n) {
    while (n--) { 
        _asm("nop"); _asm("nop"); _asm("nop"); 
    }
}

void delay_ms(unsigned int ms) {
    unsigned long i;
    for (i = 0; i < (ms * 1000UL); i++) { _asm("nop"); }
}

/* --- ДРАЙВЕР TM1637 (Open Drain) --- */

void TM1637_Start(void) {
    PC_ODR |= DIO_PIN; // 1
    PC_ODR |= CLK_PIN; // 1
    delay_us(50);
    PC_ODR &= ~DIO_PIN; // 0
    delay_us(50);
    PC_ODR &= ~CLK_PIN; // 0
}

void TM1637_Stop(void) {
    PC_ODR &= ~CLK_PIN; // 0
    delay_us(50);
    PC_ODR &= ~DIO_PIN; // 0
    delay_us(50);
    PC_ODR |= CLK_PIN;  // 1
    delay_us(50);
    PC_ODR |= DIO_PIN;  // 1
}

void TM1637_WriteByte(unsigned char b) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        PC_ODR &= ~CLK_PIN; // CLK Down
        delay_us(20);
        
        if (b & 0x01) {
            PC_ODR |= DIO_PIN; // DIO High (Open Drain відпускає лінію)
        } else {
            PC_ODR &= ~DIO_PIN; // DIO Low
        }
        
        delay_us(30);
        PC_ODR |= CLK_PIN; // CLK Up
        delay_us(50);
        
        b >>= 1;
    }
    
    // ACK (Клік)
    PC_ODR &= ~CLK_PIN; 
    PC_ODR |= DIO_PIN; 
    delay_us(20);
    PC_ODR |= CLK_PIN; 
    delay_us(50);
    PC_ODR &= ~CLK_PIN; 
}

void ShowDisplay(unsigned int num) {
    unsigned char i;
    unsigned char digits[4];

    if (num > 9999) num = 9999;
    digits[0] = num / 1000;
    digits[1] = (num / 100) % 10;
    digits[2] = (num / 10) % 10;
    digits[3] = num % 10;

    // Яскравість (0x88 = On, +0..7 = яскравість)
    
    TM1637_Start();
    TM1637_WriteByte(0x40); // Data command
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0); // Address command
    for (i = 0; i < 4; i++) {
        TM1637_WriteByte(segmentMap[digits[i]]);
    }
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0x8F); // Display ON, Max Brightness
    TM1637_Stop();
}

void main(void) {
    unsigned int counter = 0;

    CLK_CKDIVR = 0x00; // 16 MHz

    // --- НАЛАШТУВАННЯ ПІНІВ ДИСПЛЕЯ (PC3, PC4) ---
    // Використовуємо Open Drain (CR1=0)
    PC_DDR |= (CLK_PIN | DIO_PIN); // Output
    PC_CR1 &= ~(CLK_PIN | DIO_PIN); // Open Drain 
    PC_CR2 |= (CLK_PIN | DIO_PIN);  // Fast

    // Реле (PD2)
    PD_DDR |= RELAY_PIN;
    PD_CR1 &= ~RELAY_PIN; 
    PD_CR2 |= RELAY_PIN;

    while (1) {
        ShowDisplay(counter);
        
        // Блимаємо реле кожні 1000 мс
        if (counter % 10 == 0) {
             PD_ODR ^= RELAY_PIN; 
        }

        counter++;
        delay_ms(100); 
    }
}