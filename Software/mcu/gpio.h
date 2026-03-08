/**
 * @file    gpio.h
 * @brief   Драйвер портів вводу/виводу.
 */

#ifndef GPIO_H
#define GPIO_H

#include "board_config.h"

/* ========================================================================== */
/* МАКРОСИ ДЛЯ КНОПОК (Логіка Active Low: 0 - натиснута, 1 - відпущена)       */
/* ========================================================================== */
/* UP підключена до PD2 */
#define BUTTON_UP_PRESSED()   ((BTN_UP_PORT_IDR & BTN_UP_PIN) == 0)
/* DOWN підключена до PD3 */
#define BUTTON_DOWN_PRESSED() ((BTN_DOWN_PORT_IDR & BTN_DOWN_PIN) == 0)

/* SET підключена до PC5 */
#define BUTTON_SET_PRESSED()  ((BTN_SET_PORT_IDR & BTN_SET_PIN) == 0)

/* ========================================================================== */
/* МАКРОСИ ДЛЯ РЕЛЕ (Логіка Active High: 1 - увімкнено)                       */
/* ========================================================================== */
/* Реле підключене до PC3 */
#define RELAY_ON()            (RELAY_PORT_ODR |= RELAY_PIN)
#define RELAY_OFF()           (RELAY_PORT_ODR &= ~RELAY_PIN)
/**
 * @brief Ініціалізує всі піни згідно з електричною схемою термостата.
 */
void GPIO_Init(void);

#endif /* GPIO_H */