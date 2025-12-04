#include "system.h"
// --- ВАЖЛИВО: Підключаємо файл з регістрами ---
#include "iostm8s103.h" 

volatile uint32_t g_millis = 0;

// Обробник переривання таймера 4
// Додали @far, щоб воно співпадало з таблицею векторів
@far @interrupt void TIM4_UPD_OVF_IRQHandler(void) {
    g_millis++;
    TIM4_SR &= ~(1 << 0); 
}

void System_Init(void) {
    // 1. Налаштування тактування (16 МГц)
    CLK_CKDIVR = 0x00; 

    // 2. Налаштування Таймера 4 (1 мс)
    // Prescaler 128 -> 16MHz / 128 = 125 kHz
    TIM4_PSCR = 0x07; 
    // ARR 124 -> (124+1) / 125000 = 0.001s = 1ms
    TIM4_ARR = 124;   
    
    TIM4_IER |= (1 << 0); // Вмикаємо переривання (Update Interrupt)
    TIM4_CR1 |= (1 << 0); // Вмикаємо таймер
    
    _asm("rim"); // Глобально дозволяємо переривання
}

uint32_t millis(void) {
    uint32_t m;
    _asm("sim"); // Забороняємо переривання (атомарне читання)
    m = g_millis;
    _asm("rim"); // Дозволяємо переривання
    return m;
}

void delay_ms(uint32_t ms) {
    uint32_t start = millis();
    while ((millis() - start) < ms);
}