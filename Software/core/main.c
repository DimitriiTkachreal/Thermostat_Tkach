#include "iostm8s103.h"

// CLK -> PC7
// DIO -> PC6
// RELAY -> PC3

#define CLK_PIN  (1 << 7)
#define DIO_PIN  (1 << 6)
#define RELAY_PIN (1 << 3)

void delay_us(void) {
    _asm("nop"); _asm("nop"); _asm("nop"); _asm("nop");
    _asm("nop"); _asm("nop"); _asm("nop"); _asm("nop");
}

void delay_ms(unsigned int ms) {
    unsigned long i;
    for (i = 0; i < (ms * 1000UL); i++) { _asm("nop"); }
}

// --- КЕРУВАННЯ ПІНАМИ (PUSH-PULL) ---
void Pin_Low(unsigned char pin) {
    PC_ODR &= ~pin; // 0
}

void Pin_High(unsigned char pin) {
    PC_ODR |= pin;  // 1
}

// --- ПРОТОКОЛ TM1637 ---
void TM_Start(void) {
    Pin_High(DIO_PIN);
    Pin_High(CLK_PIN);
    delay_us();
    Pin_Low(DIO_PIN);
    delay_us();
    Pin_Low(CLK_PIN);
}

void TM_Stop(void) {
    Pin_Low(CLK_PIN);
    Pin_Low(DIO_PIN);
    delay_us();
    Pin_High(CLK_PIN);
    delay_us();
    Pin_High(DIO_PIN);
}

void TM_WriteByte(unsigned char b) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        Pin_Low(CLK_PIN);
        delay_us();
        
        if (b & 0x01) Pin_High(DIO_PIN);
        else          Pin_Low(DIO_PIN);
        
        delay_us();
        Pin_High(CLK_PIN);
        delay_us();
        
        b >>= 1;
    }
    
    // ACK (Просто "тік-так", ігноруємо відповідь)
    Pin_Low(CLK_PIN);
    Pin_Low(DIO_PIN); // Тут ми самі притискаємо, щоб не висіло
    delay_us();
    Pin_High(CLK_PIN);
    delay_us();
    Pin_Low(CLK_PIN);
}

void main(void) {
    unsigned int counter = 0;
    unsigned char i;
    
    CLK_CKDIVR = 0x00; // 16 MHz

    // --- НАЛАШТУВАННЯ ПІНІВ (PUSH-PULL OUTPUT) ---
    // PC3, PC6, PC7 -> Output
    PC_DDR |= (CLK_PIN | DIO_PIN | RELAY_PIN);
    // PC3, PC6, PC7 -> Push-Pull (Сильний вихід)
    PC_CR1 |= (CLK_PIN | DIO_PIN | RELAY_PIN); 
    // PC3, PC6, PC7 -> Fast mode
    PC_CR2 |= (CLK_PIN | DIO_PIN | RELAY_PIN);

    // Початковий стан
    Pin_High(CLK_PIN);
    Pin_High(DIO_PIN);

    while (1) {
        // 1. Увімкнути дисплей (Яскравість макс)
        TM_Start();
        TM_WriteByte(0x8F); // Display ON + Brightness
        TM_Stop();

        // 2. Записати дані (Авто-адреса)
        TM_Start();
        TM_WriteByte(0x40); 
        TM_Stop();

        // 3. Вивести "8888" (щоб точно побачити, чи працює)
        TM_Start();
        TM_WriteByte(0xC0); // Адреса першої цифри
        for(i=0; i<4; i++) {
            TM_WriteByte(0x7F); // Код "8" з точкою (всі сегменти)
        }
        TM_Stop();
        
        // Блимаємо реле для перевірки життя
        PC_ODR ^= RELAY_PIN;

        delay_ms(500);
    }
}