/**
 * @file    ui.h
 * @brief   Модуль інтерфейсу користувача (Меню та Екран).
 * @details Реалізує Кінцевий Автомат (FSM) для обробки кнопок та виводу на TM1637.
 */

#ifndef UI_H
#define UI_H

#include "board_config.h"

/**
 * @brief Ініціалізує інтерфейс (скидає стани).
 */
void UI_Init(void);

/**
 * @brief Головний цикл UI. Викликати у while(1) постійно.
 * @details Малює на екрані дані та обробляє натискання кнопок.
 */
void UI_Process(void);

/**
 * @brief Анімація старту пристрою. Викликати поза голвним циклом один раз.
 * @details Малює на екрані .
 */
void UI_StartAnimation(void);

#endif /* UI_H */