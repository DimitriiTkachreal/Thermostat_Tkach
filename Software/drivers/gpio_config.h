#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#include "iostm8s103.h"

// --- ДИСПЛЕЙ (TM1637) ---
// Змінив згідно твоїх слів: CLK -> PC7, DIO -> PC6
#define DISP_PORT_ODR   PC_ODR
#define DISP_PORT_DDR   PC_DDR
#define DISP_PORT_CR1   PC_CR1
#define DISP_PORT_CR2   PC_CR2
#define DISP_CLK_PIN    (1 << 7)
#define DISP_DIO_PIN    (1 << 6)

// --- РЕЛЕ ---
// Змінив: PC3
#define RELAY_PORT_ODR  PC_ODR
#define RELAY_PORT_DDR  PC_DDR
#define RELAY_PORT_CR1  PC_CR1
#define RELAY_PORT_CR2  PC_CR2
#define RELAY_PIN       (1 << 3)

// --- NTC ДАТЧИК ---
// Змінив: PC4 (Це канал AIN2)
#define NTC_PORT_DDR    PC_DDR
#define NTC_PORT_CR1    PC_CR1
#define NTC_PORT_CR2    PC_CR2
#define NTC_PIN         (1 << 4)
// Для ADC треба знати номер каналу. PC4 - це AIN2.
#define NTC_ADC_CHANNEL 2 

#endif