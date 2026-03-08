/**
 * @file    uart.h
 * @brief   Драйвер UART для виводу налагоджувальної інформації (Debug).
 * @details Налаштований на передачу даних (TX) зі швидкістю 115200 бод.
 */

#ifndef UART_H
#define UART_H

#include "board_config.h" 

/**
 * @brief Ініціалізує апаратний блок UART (Тільки передавач).
 */
void UART_Init(void);

/**
 * @brief Відправляє один символ (букву) у термінал.
 * @param ch Символ для відправки (наприклад, 'A' або '\n')
 */
void UART_SendChar(char ch);

/**
 * @brief Відправляє цілий текстовий рядок.
 * @param str Вказівник на рядок (наприклад, "Hello\n")
 */
void UART_SendString(const char *str);

/**
 * @brief Відправляє число (допоміжна функція для логування температур).
 * @param num Число для відправки (наприклад, 250)
 */
void UART_SendInt(int16_t num);

#endif /* UART_H */