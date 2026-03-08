/**
 * @file    buttons.c
 * @brief   Реалізація кінцевого автомата (FSM) для кнопок.
 */

#include "buttons.h"
#include "gpio.h"  /* Для макросів BUTTON_UP_PRESSED() */
#include "timer.h" /* Для функції Timer_GetMillis() */

/* ========================================================================== */
/* ВНУТРІШНІ ТИПИ ДАНИХ (ПРИХОВАНІ ВІД ІНШИХ ФАЙЛІВ)                          */
/* ========================================================================== */

/* Стани кінцевого автомата */
typedef enum
{
    STATE_IDLE = 0,
    STATE_DEBOUNCE,
    STATE_PRESSED,
    STATE_REPEAT
} btn_state_t;

/* Контекст (пам'ять) для кожної кнопки */
typedef struct
{
    btn_state_t    state;         /* В якому стані кнопка зараз */
    uint32_t       start_time;    /* Позначка часу для розрахунку затримок */
    button_event_t current_event; /* Подія, що чекає на зчитування */
} button_context_t;

/* Масив кнопок */
static button_context_t btns[BTN_COUNT];

/* ========================================================================== */
/* ВНУТРІШНІ ФУНКЦІЇ                                                          */
/* ========================================================================== */

/* Зчитує фізичний рівень на піні. Повертає 1 якщо натиснуто, 0 якщо ні. */
static uint8_t HW_ReadButton(button_id_t id) 
{
    switch (id)
    {
        case BTN_UP:   return BUTTON_UP_PRESSED();
        case BTN_DOWN: return BUTTON_DOWN_PRESSED();
        case BTN_SET:  return BUTTON_SET_PRESSED();
        default:       return 0;
    }
}

/* ========================================================================== */
/* ПУБЛІЧНІ ФУНКЦІЇ (API)                                                     */
/* ========================================================================== */

void Buttons_Init(void) 
{
    uint8_t i;
    for (i = 0; i < (uint8_t)BTN_COUNT; i++)
    {
        btns[i].state = STATE_IDLE;
        btns[i].start_time = 0;
        btns[i].current_event = BTN_EVENT_NONE;
    }
}

button_event_t Buttons_GetEvent(button_id_t id) 
{
    button_event_t ev;
    if (id >= BTN_COUNT) return BTN_EVENT_NONE;
    
    /* Атомарно забираємо подію (читаємо і стираємо) */
    ev = btns[id].current_event;
    btns[id].current_event = BTN_EVENT_NONE;
    
    return ev;
}

/* ========================================================================== */
/* БІЗНЕС-ЛОГІКА: КІНЦЕВИЙ АВТОМАТ (FSM)                                      */
/* ========================================================================== */

void Buttons_Process(void) 
{
    uint8_t i;
    uint8_t is_pressed;
    uint32_t current_time;
    
    /* Читаємо глобальний час тільки один раз на весь цикл опитування */
    current_time = Timer_GetMillis();

    for (i = 0; i < (uint8_t)BTN_COUNT; i++) 
    {
        is_pressed = HW_ReadButton((button_id_t)i);

        switch (btns[i].state) 
        {
            case STATE_IDLE:
                if (is_pressed)
                {
                    btns[i].start_time = current_time;
                    btns[i].state = STATE_DEBOUNCE;
                }
                break;

            case STATE_DEBOUNCE:
                if (is_pressed)
                {
                    if ((current_time - btns[i].start_time) >= 20)
                    {
                        btns[i].state = STATE_PRESSED;
                    }
                }
                else
                {
                    btns[i].state = STATE_IDLE;
                }
                break;

            case STATE_PRESSED:
                if (is_pressed)
                {
                    if ((current_time - btns[i].start_time) >= 1000)
                    {
                        btns[i].current_event = BTN_EVENT_HOLD;
                        btns[i].start_time = current_time;
                        btns[i].state = STATE_REPEAT;
                    }
                }
                else
                {
                    btns[i].current_event = BTN_EVENT_CLICK;
                    btns[i].state = STATE_IDLE;
                }
                break;

            case STATE_REPEAT:
                if (is_pressed)
                {
                    if ((current_time - btns[i].start_time) >= 100)
                    {
                        btns[i].current_event = BTN_EVENT_HOLD;
                        btns[i].start_time = current_time;
                    }
                }
                else
                {
                    btns[i].state = STATE_IDLE;
                }
                break;
        }
    }
}