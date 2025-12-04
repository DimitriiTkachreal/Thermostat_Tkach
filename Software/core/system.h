#ifndef SYSTEM_H
#define SYSTEM_H

// Тут НЕ треба підключати iostm8s103.h
// Використовуємо стандартні типи, якщо треба
typedef unsigned long uint32_t;

void System_Init(void);
uint32_t millis(void);
void delay_ms(uint32_t ms);

#endif