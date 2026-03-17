/**
 * @file    clock.h
 * @brief   Драйвер системи тактування (Clock Controller).
 * @details Містить прототипи функцій для налаштування головної 
 * частоти мікроконтролера.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "mcu_regs.h"
#define CLK_CKDIVR_HSIDIV_MASK  ((uint8_t)((1 << 3) | (1 << 4)))
/**
 * @brief   Ініціалізує систему тактування.
 * @details Знімає заводський дільник HSI (High-Speed Internal) генератора.
 * Переводить мікроконтролер з частоти 2 МГц на максимальні 16 МГц.
 * Викликається найпершою у функції main().
 * @param   None
 * @return  None
 */
void CLOCK_Init(void);

/**
 * @brief   Програмна затримка в мікросекундах.
 * @details Блокуюча затримка на базі порожніх циклів процесора (NOP).
 * Розрахована суворо для частоти ядра 16 МГц (де 1 такт = 62.5 нс).
 * @param   us Кількість мікросекунд для очікування.
 * @return  None
 */
void delay_us(uint32_t us);

#endif /* CLOCK_H */