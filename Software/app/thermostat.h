/**
 * @file    thermostat.h
 * @brief   Головний контролер логіки термостата (Бізнес-логіка).
 * @details Керує реле на основі поточної температури та налаштувань P0-P7.
 */

#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "board_config.h"

/*Межі роботи пристрою*/
#define HARDWARE_TEMP_MAX  1200  /* 120.0 C */
#define HARDWARE_TEMP_MIN -200   /* -20.0 C */
/* Стани термостата для передачі в модуль екрана (ui.c) */
typedef enum {
    THERM_STATE_NORMAL = 0, /* Працює штатно */
    THERM_STATE_ALARM,      /* Аварія (перевищено P6) */
    THERM_STATE_SENSOR_ERR  /* Обрив або замикання датчика */
} therm_state_t;

/**
 * @brief Ініціалізує початковий стан термостата (вимикає реле).
 */
void Thermostat_Init(void);

/**
 * @brief Головний цикл прийняття рішень. Викликати у while(1).
 */
void Thermostat_Process(void);

/**
 * @brief Повертає поточну відкалібровану температуру.
 */
int16_t Thermostat_GetCurrentTemp(void);

/**
 * @brief Повертає глобальний стан системи (Норма, Аварія, Помилка).
 */
therm_state_t Thermostat_GetState(void);

#endif /* THERMOSTAT_H */