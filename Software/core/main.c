/**
 * @file    main.c
 * @brief   Головний файл проєкту (Точка входу).
 * @details Ініціалізує всі модулі та запускає нескінченний цикл опитування (Super-loop).
 */
#include "board_config.h"
#include "clock.h"
#include "gpio.h"
#include "timer.h"
#include "buttons.h"
#include "ntc.h"
#include "adc.h"
#include "tm1637.h"
#include "eeprom.h"
#include "settings.h"
#include "thermostat.h"
#include "ui.h"
#include "uart.h"

/* Змінна для неблокуючої відправки телеметрії (логування) */
static uint32_t last_log_time = 0;
/*Для оновлення дисплея*/
static uint32_t last_ui_update = 0;

void main(void)
{
    /* ====================================================================== */
    /* 1. ІНІЦІАЛІЗАЦІЯ ЗАЛІЗА (Hardware Abstraction Layer)                   */
    /* ====================================================================== */
    /* Важливо: Годинник завжди запускається першим! */
    CLOCK_Init();  
    GPIO_Init();   
    Timer4_Init();  
    UART_Init();   
    ADC_Init();

    _asm("rim");
    
    /* ====================================================================== */
    /* 2. ІНІЦІАЛІЗАЦІЯ БІЗНЕС-ЛОГІКИ (Business Logic Layer)                  */
    /* ====================================================================== */
    /* Важливо: Settings_Init має бути до Thermostat_Init, бо мозок читає EEPROM */
    Settings_Init();     
    Thermostat_Init();   
    UI_Init();   
    
    UI_StartAnimation();//Напис старт

    /* Сигнал у термінал про успішне завантаження (Телеметрія) */
    UART_SendString("\n--- Thermostat Boot OK ---\n");

    /* ====================================================================== */
    /* 3. ГОЛОВНИЙ ЦИКЛ (Super-loop / Round-Robin)                            */
    /* ====================================================================== */
    while (1)
    {
        uint32_t current_time = Timer_GetMillis();

        /* А) Збір даних із зовнішнього світу */
        Buttons_Process();
        if ((current_time - last_ui_update) >= 250) 
        {
        /* Б) Обробка фізики та керування реле */
        Thermostat_Process();
        /* В) Обробка меню та малювання на екрані */
        UI_Process();
        last_ui_update = current_time;
        }
        /* Г) Телеметрія (Відправляємо лог у термінал комп'ютера кожні 2 секунди) */
        current_time = Timer_GetMillis();
        if ((current_time - last_log_time) >= 2000) 
        {
            UART_SendString("T: ");
            UART_SendInt(Thermostat_GetCurrentTemp());
            UART_SendString(" | Set: ");
            UART_SendInt(Settings_Get(PARAM_TARGET_TEMP));
            UART_SendString(" | State: ");
            UART_SendInt((int16_t)Thermostat_GetState());
            UART_SendString("\n");
            
            last_log_time = current_time;
        }
    }
}