/**
 * @file    board_config.h
 * @brief   Карта підключення апаратної частини (Піни термостата v1.0).
 * @details Базується на принциповій схемі. Всі піни і порти описані тут.
 */

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "mcu_regs.h"

/* ========================================================================== */
/* 1. ДИСПЛЕЙ TM1637                                                          */
/* ========================================================================== */
/* DIO підключено до PC6, CLK підключено до PC7 */
#define TM1637_PORT_ODR    GPIOC_ODR
#define TM1637_PORT_IDR    GPIOC_IDR
#define TM1637_PORT_DDR    GPIOC_DDR
#define TM1637_PORT_CR1    GPIOC_CR1
#define TM1637_PORT_CR2    GPIOC_CR2

#define TM1637_DIO_PIN     (1 << 6) /* PC6 */
#define TM1637_CLK_PIN     (1 << 7) /* PC7 */

/* ========================================================================== */
/* 2. ТЕРМІСТОР NTC 10K (АЦП)                                                 */
/* ========================================================================== */
/* Підключено до PC4 (Канал АЦП 2) */
#define NTC_ADC_CHANNEL    (1 << 1) /* Двійкове 0010 для регістра CSR */
#define NTC_PORT_DDR       GPIOC_DDR
#define NTC_PORT_CR1       GPIOC_CR1
#define NTC_PORT_CR2       GPIOC_CR2
#define NTC_PIN            (1 << 4) /* Фізичний пін PC4 */

/* ========================================================================== */
/* 3. КНОПКИ КЕРУВАННЯ                                                        */
/* ========================================================================== */
/* UP -> PD2, DOWN -> PD3, SET -> PC5 */
#define BTN_UP_PORT_IDR    GPIOD_IDR
#define BTN_UP_PIN         (1 << 2)

#define BTN_DOWN_PORT_IDR  GPIOD_IDR
#define BTN_DOWN_PIN       (1 << 3)

#define BTN_SET_PORT_IDR   GPIOC_IDR
#define BTN_SET_PIN        (1 << 5)

/* ========================================================================== */
/* 4. РЕЛЕ НАГРІВАЧА/ОХОЛОДЖУВАЧА                                             */
/* ========================================================================== */
/* IN підключено до PC3 */
#define RELAY_PORT_ODR     GPIOC_ODR
#define RELAY_PORT_DDR     GPIOC_DDR
#define RELAY_PORT_CR1     GPIOC_CR1
#define RELAY_PORT_CR2     GPIOC_CR2

#define RELAY_PIN          (1 << 3) /* PC3 */

#endif /* BOARD_CONFIG_H */