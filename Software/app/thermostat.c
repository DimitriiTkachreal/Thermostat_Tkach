/**
 * @file    thermostat.c
 * @brief   Реалізація головної бізнес-логіки.
 */

#include "thermostat.h"
#include "settings.h"
#include "ntc.h"
#include "timer.h"
#include "gpio.h"

/* ========================================================================== */
/* ВНУТРІШНІ ЗМІННІ                                                           */
/* ========================================================================== */

static int16_t current_calibrated_temp = 0;
static therm_state_t system_state = THERM_STATE_NORMAL;

/* Змінні для розрахунку затримки компресора (P5) */
static uint8_t  is_relay_on = 0;
static uint32_t time_relay_turned_off = 0;

/* ========================================================================== */
/* ПРИВАТНІ ФУНКЦІЇ                                                           */
/* ========================================================================== */

/* Фізичне вмикання реле з урахуванням затримки P5 */
static void TurnRelayOn(void) 
{
    uint32_t delay_ms;
    uint32_t current_time;
    
    if (is_relay_on) return; /* Вже увімкнено */

    /* Читаємо налаштування затримки P5 (у хвилинах). Переводимо в мілісекунди. */
    delay_ms = (uint32_t)Settings_Get(PARAM_P5_DELAY) * 60000UL;
    current_time = Timer_GetMillis();

    /* Перевіряємо, чи минуло достатньо часу з моменту останнього вимкнення */
    if ((current_time - time_relay_turned_off) >= delay_ms) 
    {
        RELAY_ON();
        is_relay_on = 1;
    }
}

/* Фізичне вимикання реле */
static void TurnRelayOff(void) 
{
    if (!is_relay_on) return; /* Вже вимкнено */

    RELAY_OFF();
    is_relay_on = 0;
    
    /* Запам'ятовуємо час вимкнення для відліку затримки P5 */
    time_relay_turned_off = Timer_GetMillis();
}

/* ========================================================================== */
/* ПУБЛІЧНІ ФУНКЦІЇ (API)                                                     */
/* ========================================================================== */

void Thermostat_Init(void) 
{
    RELAY_OFF();
    is_relay_on = 0;
    system_state = THERM_STATE_NORMAL;
    time_relay_turned_off = Timer_GetMillis(); /* Починаємо відлік від старту */
}

int16_t Thermostat_GetCurrentTemp(void) 
{
    return current_calibrated_temp;
}

therm_state_t Thermostat_GetState(void) 
{
    return system_state;
}

/* ГОЛОВНИЙ МОЗОК */
void Thermostat_Process(void) 
{
   int16_t raw_temp;
   int16_t alarm_limit;
   int16_t target, hyst, mode;
   
    /* 1. Читаємо датчик і застосовуємо калібрування P4 */
    raw_temp = NTC_GetTemperature();
    
    /* Якщо датчик відірвали (NTC_GetTemp повертає помилку, наприклад -999 або дуже велике число) */
    if (raw_temp <= -500 || raw_temp >= 1300) 
    {
        TurnRelayOff();
        system_state = THERM_STATE_SENSOR_ERR;
        return; /* Припиняємо роботу */
    }

    current_calibrated_temp = raw_temp + Settings_Get(PARAM_P4_CALIB);

    /* 2. Захист від пожежі (ALARM P6) */
    alarm_limit = Settings_Get(PARAM_P6_ALARM);
    if (current_calibrated_temp >= alarm_limit) 
    {
        TurnRelayOff();
        system_state = THERM_STATE_ALARM;
        return; /* Блокуємо роботу, поки не охолоне */
    }

    /* Якщо все добре - працюємо в штатному режимі */
    system_state = THERM_STATE_NORMAL;

    /* 3. Отримуємо робочі параметри з пам'яті */
    target = Settings_Get(PARAM_TARGET_TEMP);
    hyst   = Settings_Get(PARAM_P1_HYST);
    mode   = Settings_Get(PARAM_P0_MODE); /* 0 = HEAT, 1 = COOL */

    /* 4. Бізнес-логіка (Гістерезис) */
    if (mode == 0) /* РЕЖИМ НАГРІВУ (HEAT) */
    {
        if (current_calibrated_temp <= (target - hyst)) 
        {
            TurnRelayOn();  /* Холодно -> Гріємо */
        } 
        else if (current_calibrated_temp >= target) 
        {
            TurnRelayOff(); /* Нагріли -> Вимикаємо */
        }
    }
    else /* РЕЖИМ ОХОЛОДЖЕННЯ (COOL) */
    {
        if (current_calibrated_temp >= (target + hyst)) 
        {
            TurnRelayOn();  /* Жарко -> Охолоджуємо */
        } 
        else if (current_calibrated_temp <= target) 
        {
            TurnRelayOff(); /* Охолодили -> Вимикаємо */
        }
    }
}