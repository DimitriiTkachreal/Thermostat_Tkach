/**
 * @file mcu_regs.h
 * @brief Карта регістрів мікроконтролера STM8S103F3P6.
 */

#ifndef MCU_REGS_H
#define MCU_REGS_H

/* ========================================================================== */
/* 1. БАЗОВІ ТИПИ ДАНИХ                                                       */
/* ========================================================================== */
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned long      uint32_t;
typedef signed long        int32_t;

/* ========================================================================== */
/* 2. РЕГІСТРИ ПОРТІВ ВВОДУ/ВИВОДУ (GPIO)                                     */
/* ========================================================================== */

/* PORT A */
#define GPIOA_ODR   (*(volatile uint8_t *)0x5000) /* Data Output Latch */
#define GPIOA_IDR   (*(volatile uint8_t *)0x5001) /* Input Pin Value */
#define GPIOA_DDR   (*(volatile uint8_t *)0x5002) /* Data Direction */
#define GPIOA_CR1   (*(volatile uint8_t *)0x5003) /* Control Register 1 */
#define GPIOA_CR2   (*(volatile uint8_t *)0x5004) /* Control Register 2 */

/* PORT B */
#define GPIOB_ODR   (*(volatile uint8_t *)0x5005)
#define GPIOB_IDR   (*(volatile uint8_t *)0x5006)
#define GPIOB_DDR   (*(volatile uint8_t *)0x5007)
#define GPIOB_CR1   (*(volatile uint8_t *)0x5008)
#define GPIOB_CR2   (*(volatile uint8_t *)0x5009)

/* PORT C */
#define GPIOC_ODR   (*(volatile uint8_t *)0x500A)
#define GPIOC_IDR   (*(volatile uint8_t *)0x500B)
#define GPIOC_DDR   (*(volatile uint8_t *)0x500C)
#define GPIOC_CR1   (*(volatile uint8_t *)0x500D)
#define GPIOC_CR2   (*(volatile uint8_t *)0x500E)

/* PORT D */
#define GPIOD_ODR   (*(volatile uint8_t *)0x500F)
#define GPIOD_IDR   (*(volatile uint8_t *)0x5010)
#define GPIOD_DDR   (*(volatile uint8_t *)0x5011)
#define GPIOD_CR1   (*(volatile uint8_t *)0x5012)
#define GPIOD_CR2   (*(volatile uint8_t *)0x5013)

/* ========================================================================== */
/* 3. РЕГІСТРИ СИСТЕМИ ТАКТУВАННЯ (CLOCK)                                     */
/* ========================================================================== */
#define CLK_CKDIVR  (*(volatile uint8_t *)0x50C6) /* Clock divider register */
#define CLK_PCKENR1 (*(volatile uint8_t *)0x50C7) /* Peripheral clock gating 1 */
#define CLK_PCKENR2 (*(volatile uint8_t *)0x50C8) /* Peripheral clock gating 2 */

/* ========================================================================== */
/* 4. РЕГІСТРИ СИСТЕМНОГО ТАЙМЕРА (TIM4)                                      */
/* ========================================================================== */
#define TIM4_CR1    (*(volatile uint8_t *)0x5340) /* Control Register 1 */
#define TIM4_IER    (*(volatile uint8_t *)0x5343) /* Interrupt Enable Register */
#define TIM4_SR     (*(volatile uint8_t *)0x5344) /* Status Register */
#define TIM4_EGR    (*(volatile uint8_t *)0x5345) /* Event Generation Register */
#define TIM4_CNTR   (*(volatile uint8_t *)0x5346) /* Counter Register */
#define TIM4_PSCR   (*(volatile uint8_t *)0x5347) /* Prescaler Register */
#define TIM4_ARR    (*(volatile uint8_t *)0x5348) /* Auto-Reload Register */

/* ========================================================================== */
/* 5. РЕГІСТРИ АЦП (ADC1) - ДЛЯ ТЕРМІСТОРА                                    */
/* ========================================================================== */
#define ADC1_CSR    (*(volatile uint8_t *)0x5400) /* Control/Status Register */
#define ADC1_CR1    (*(volatile uint8_t *)0x5401) /* Configuration Register 1 */
#define ADC1_CR2    (*(volatile uint8_t *)0x5402) /* Configuration Register 2 */
#define ADC1_DRH    (*(volatile uint8_t *)0x5404) /* Data Register High */
#define ADC1_DRL    (*(volatile uint8_t *)0x5405) /* Data Register Low */

/* ========================================================================== */
/* 6. РЕГІСТРИ КОНТРОЛЕРА ПАМ'ЯТІ (FLASH / EEPROM)                            */
/* ========================================================================== */
#define FLASH_IAPSR (*(volatile uint8_t *)0x505F) /* Регістр статусу */
#define FLASH_DUKR  (*(volatile uint8_t *)0x5064) /* Регістр введення ключів (Data Unlock) */

/* Прапорець: Data EEPROM area unlocked (1 - відкрито, 0 - закрито) */
#define FLASH_IAPSR_DUL ((uint8_t)(1 << 3))
/* Початкова адреса Data EEPROM для STM8S103F3 */
#define EEPROM_START_ADDR ((uint16_t)0x4000)

/* ========================================================================== */
/* 7. РЕГІСТРИ UART1 (Для телеметрії та логування)                            */
/* ========================================================================== */
#define UART1_SR   (*(volatile uint8_t *)0x5230) /* Регістр статусу */
#define UART1_DR   (*(volatile uint8_t *)0x5231) /* Регістр даних (сюди кладемо букву) */
#define UART1_BRR1 (*(volatile uint8_t *)0x5232) /* Регістр швидкості 1 */
#define UART1_BRR2 (*(volatile uint8_t *)0x5233) /* Регістр швидкості 2 */
#define UART1_CR2  (*(volatile uint8_t *)0x5235) /* Регістр керування 2 */

/* Прапорець статусу: Transmit Data Register Empty (буфер порожній, можна писати) */
#define UART1_SR_TXE ((uint8_t)(1 << 7))
/* Прапорець керування: Transmitter Enable (увімкнути передавач) */
#define UART1_CR2_TEN ((uint8_t)(1 << 3))

#endif /* MCU_REGS_H */