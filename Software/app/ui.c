/**
 * @file    ui.c
 * @brief   Реалізація меню термостата.
 */

#include "ui.h"
#include "tm1637.h"
#include "buttons.h"
#include "thermostat.h"
#include "settings.h"
#include "timer.h"

/* ========================================================================== */
/* СЛОВНИК СИМВОЛІВ ТА ЦИФР                                                   */
/* ========================================================================== */
static const uint8_t DIGIT_CODES[10] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F  /* 9 */
};

#define SYM_BLANK 0x00
#define SYM_MINUS 0x40
#define SYM_P 0x73
#define SYM_H 0x76
#define SYM_E 0x79
#define SYM_A 0x77
#define SYM_t 0x78
#define SYM_C 0x39
#define SYM_o 0x5C
#define SYM_L 0x38
#define SYM_r 0x50

static const uint8_t MSG_ERROR[4] = {SYM_E, SYM_r, SYM_r, SYM_BLANK};

/* ========================================================================== */
/* СТАНИ АВТОМАТА (FSM)                                                       */
/* ========================================================================== */
typedef enum
{
    STATE_NORMAL = 0,   /* Поточна температура */
    STATE_QUICK_SET,    /* Блимає і змінюється цільова температура */
    STATE_MENU_SELECT,  /* Вибір параметра P0-P7 */
    STATE_MENU_EDIT     /* Зміна конкретного параметра */
} ui_state_t;

/* Внутрішня пам'ять інтерфейсу */
static ui_state_t current_state = STATE_NORMAL;
static param_id_t current_param_index = PARAM_P0_MODE; /* На якому P ми стоїмо */
static int16_t    edit_value = 0;                      /* Тимчасова змінна для редагування */

/* Таймер для блимання екрана */
static uint32_t last_blink_time = 0;
static uint8_t  is_screen_on = 1;

/* Таймер для авто-виходу з меню (якщо користувач забув натиснути SET) */
static uint32_t last_activity_time = 0;
#define AUTO_EXIT_TIMEOUT_MS 10000 /* 10 секунд бездіяльності = вихід */

/* ========================================================================== */
/* ПРИВАТНІ ФУНКЦІЇ (Малювання)                                               */
/* ========================================================================== */

/* Керує блиманням (повертає 1, якщо треба малювати, і 0, якщо треба гасити екран) */
static uint8_t UI_BlinkControl(void) 
{
    uint32_t current_time = Timer_GetMillis();
    if ((current_time - last_blink_time) >= 300) /* Блимаємо кожні 300 мс */
    {
        is_screen_on = !is_screen_on; /* Інвертуємо стан */
        last_blink_time = current_time;
    }
    return is_screen_on;
}

/* Малює слова типу HEAT, COOL, ALrM */
static void UI_DrawWord(param_id_t param, int16_t val) 
{
    if (param == PARAM_P0_MODE) 
    {
        if (val == 0) TM1637_ShowRaw(SYM_H, SYM_E, SYM_A, SYM_t); /* HEAT */
        else          TM1637_ShowRaw(SYM_C, SYM_o, SYM_o, SYM_L); /* COOL */
    }
}
/*Напис при старті пристрою*/
void UI_StartAnimation(void) 
{
    uint8_t i;
    /* Малюємо риски, що бігають по колу (або сегментах) */
    uint8_t anim[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20}; 
    
    for(i = 0; i < 12; i++)
    {
        /* Ефект "змійки" на всіх розрядах */
        TM1637_ShowRaw(anim[i%6], anim[i%6], anim[i%6], anim[i%6]);
        delay_us(10000);
    }
    
    /* Напис старт*/
    TM1637_ShowRaw(0x6D, 0x78, 0x50, 0x78);
    delay_us(150000);
}

/* Розбиває температуру на цифри і відправляє на екран */
static void UI_DrawTemp(int16_t temp) 
{
    uint8_t d0, d1, d2, d3;
    uint8_t is_negative = 0;
    uint8_t tenths, ones, tens;

    if (temp < 0)
    {
        is_negative = 1;
        temp = -temp;
    }

    tenths = temp % 10;
    ones   = (temp / 10) % 10;
    tens   = (temp / 100) % 10;

    d0 = is_negative ? SYM_MINUS : SYM_BLANK; 
    d1 = (tens == 0 && !is_negative) ? SYM_BLANK : DIGIT_CODES[tens];
    d2 = DIGIT_CODES[ones] | 0x80; /* Накладаємо крапку */
    d3 = DIGIT_CODES[tenths];
    
    TM1637_ShowRaw(d0, d1, d2, d3);
}

/* Виводить напис Err */
static void UI_DrawError(void) 
{
    TM1637_ShowRaw(MSG_ERROR[0], MSG_ERROR[1], MSG_ERROR[2], MSG_ERROR[3]);
}

/* ========================================================================== */
/* ГОЛОВНИЙ ПРОЦЕС (Бізнес-логіка UI)                                         */
/* ========================================================================== */

void UI_Init(void) 
{
    current_state = STATE_NORMAL;
    last_activity_time = Timer_GetMillis();
}

void UI_Process(void) 
{
    button_event_t event_set;
    button_event_t event_up;
    button_event_t event_down;

    /* Читаємо події кнопок */
    event_set  = Buttons_GetEvent(BTN_SET);
    event_up   = Buttons_GetEvent(BTN_UP);
    event_down = Buttons_GetEvent(BTN_DOWN);

    /* Записуємо час будь-якої активності для авто-виходу */
    if (event_set || event_up || event_down) {
        last_activity_time = Timer_GetMillis();
        is_screen_on = 1; /* При натисканні екран завжди загоряється */
        last_blink_time = Timer_GetMillis();
    }

    /* Авто-вихід з меню через 10 секунд бездіяльності */
    if (current_state != STATE_NORMAL)
    {
        if ((Timer_GetMillis() - last_activity_time) >= AUTO_EXIT_TIMEOUT_MS)
        {
            current_state = STATE_NORMAL;
        }
    }

    /* ============================================================ */
    /* МАШИНА СТАНІВ (FSM)                                          */
    /* ============================================================ */
    switch (current_state) 
    {
        /* -------------------------------------------------------- */
        case STATE_NORMAL:
        {
            /* Аварія? Малюємо ALrM і блокуємо меню */
            if (Thermostat_GetState() == THERM_STATE_ALARM)
            {
                if (UI_BlinkControl()) TM1637_ShowRaw(SYM_A, SYM_L, SYM_r, SYM_BLANK);
                else TM1637_ShowRaw(SYM_BLANK, SYM_BLANK, SYM_BLANK, SYM_BLANK);
                return; /* Далі не йдемо */
            }
            if (Thermostat_GetState() == THERM_STATE_SENSOR_ERR)
            {
                UI_DrawError(); /* Напис Err */
                return;
            }

            /* Нормальний режим: просто малюємо температуру */
            UI_DrawTemp(Thermostat_GetCurrentTemp());

            /* Переходи */
            if (event_set == BTN_EVENT_CLICK)
            {
                /* Короткий клік - швидке налаштування цілі */
                edit_value = Settings_Get(PARAM_TARGET_TEMP);
                current_state = STATE_QUICK_SET;
            } 
            else if (event_set == BTN_EVENT_HOLD)
            {
                /* Довге затискання (2 сек) - вхід у глибоке меню */
                current_param_index = PARAM_P0_MODE; /* Починаємо з P0 */
                current_state = STATE_MENU_SELECT;
            }
            break;
        }

        /* -------------------------------------------------------- */
        case STATE_QUICK_SET:
        {
            /* Малюємо цільову температуру з блиманням */
            if (UI_BlinkControl()) UI_DrawTemp(edit_value);
            else TM1637_ShowRaw(SYM_BLANK, SYM_BLANK, SYM_BLANK, SYM_BLANK);

            /* Логіка кнопок */
            if (event_up == BTN_EVENT_CLICK || event_up == BTN_EVENT_HOLD)
            {
                edit_value += 1; /* Крок 0.1 °C */
                if (edit_value > Settings_Get(PARAM_P2_MAX_LIMIT)) 
                    edit_value = Settings_Get(PARAM_P2_MAX_LIMIT); /* Захист P2 */
            }
            if (event_down == BTN_EVENT_CLICK || event_down == BTN_EVENT_HOLD)
            {
                edit_value -= 1;
                if (edit_value < Settings_Get(PARAM_P3_MIN_LIMIT)) 
                    edit_value = Settings_Get(PARAM_P3_MIN_LIMIT); /* Захист P3 */
            }
            if (event_set == BTN_EVENT_CLICK)
            {
                /* Зберігаємо і виходимо */
                Settings_Set(PARAM_TARGET_TEMP, edit_value);
                current_state = STATE_NORMAL;
            }
            break;
        }

        /* -------------------------------------------------------- */
        case STATE_MENU_SELECT:
        {
            /* Малюємо ім'я параметра: "P0  ", "P1  " */
            uint8_t digit_code = SYM_BLANK;
            uint8_t display_index = (uint8_t)current_param_index - 1; /* Виправляємо зміщення P0 */
            
            if (display_index <= 9)
            {
                digit_code = DIGIT_CODES[display_index];
            }
            TM1637_ShowRaw(SYM_P, digit_code, SYM_BLANK, SYM_BLANK);

            /* Гортаємо параметри P0-P7 */
            if (event_up == BTN_EVENT_CLICK)
            {
                current_param_index++;
                if (current_param_index >= PARAM_COUNT) current_param_index = PARAM_P0_MODE;
            }
            if (event_down == BTN_EVENT_CLICK)
            {
                if (current_param_index == PARAM_P0_MODE) current_param_index = PARAM_P6_ALARM; /* P7 ми зробимо окремо, поки P0-P6 */
                else current_param_index--;
            }
            if (event_set == BTN_EVENT_CLICK)
            {
                /* Заходимо в редагування конкретного параметра */
                edit_value = Settings_Get(current_param_index);
                current_state = STATE_MENU_EDIT;
            }
            break;
        }

        /* -------------------------------------------------------- */
        case STATE_MENU_EDIT:
        {
            /* Малюємо значення*/
            if (UI_BlinkControl())
            {
                if (current_param_index == PARAM_P0_MODE)
                {
                    UI_DrawWord(current_param_index, edit_value);
                }
                else if (current_param_index == PARAM_P5_DELAY)
                {
                    /* Затримка малюється без крапки (цілі хвилини) */
                    UI_DrawTemp(edit_value * 10); 
                }
                else
                {
                    /* Всі інші малюються з крапкою */
                    UI_DrawTemp(edit_value);
                }
            }
            else
            {
                TM1637_ShowRaw(SYM_BLANK, SYM_BLANK, SYM_BLANK, SYM_BLANK);
            }

            /* Логіка кнопок для зміни значення */
            if (event_up == BTN_EVENT_CLICK || event_up == BTN_EVENT_HOLD)
            {
                edit_value += 1; /* Крок 0.1 або 1 */
            }
            if (event_down == BTN_EVENT_CLICK || event_down == BTN_EVENT_HOLD)
            {
                edit_value -= 1;
            }
            
            if (event_up || event_down) /* Якщо значення змінилося, перевіряємо межі */
            {
                int16_t min_limit = HARDWARE_TEMP_MIN;
                int16_t max_limit = HARDWARE_TEMP_MAX;

                /* Визначаємо межі залежно від поточного параметра */
                switch (current_param_index)
                {
                    case PARAM_P0_MODE:      min_limit = 0; max_limit = 1; break; /* 0-Heat, 1-Cool */
                    case PARAM_P1_HYST:      min_limit = 1; max_limit = 150; break; /* 0.1 ... 15.0 °C */
                    case PARAM_P2_MAX_LIMIT: min_limit = Settings_Get(PARAM_P3_MIN_LIMIT); max_limit = HARDWARE_TEMP_MAX; break;
                    case PARAM_P3_MIN_LIMIT: min_limit = HARDWARE_TEMP_MIN; max_limit = Settings_Get(PARAM_P2_MAX_LIMIT); break;
                    case PARAM_P4_CALIB:     min_limit = -100; max_limit = 100; break; /* -10.0 ... 10.0 °C */
                    case PARAM_P5_DELAY:     min_limit = 0; max_limit = 10; break; /* 0 ... 10 хв */
                    case PARAM_P6_ALARM:     min_limit = HARDWARE_TEMP_MIN; max_limit = HARDWARE_TEMP_MAX; break;
                    default: break; 
                }

                /* Застосовуємо жорстке обрізання (кліппінг) */
                if (edit_value > max_limit) edit_value = max_limit;
                if (edit_value < min_limit) edit_value = min_limit;
            }
            
            if (event_set == BTN_EVENT_CLICK)
            {
                /* Зберігаємо нове значення в EEPROM */
                Settings_Set(current_param_index, edit_value);
                /* Повертаємося до вибору "P..." */
                current_state = STATE_MENU_SELECT;
            }
            break;
        }
    }
}