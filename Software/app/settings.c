/**
 * @file    settings.c
 * @brief   Реалізація кешування та запису в EEPROM.
 */

#include "settings.h"
#include "eeprom.h"

/* ========================================================================== */
/* КАРТА ПАМ'ЯТІ (Де саме в 0x4000 лежать наші байти)                         */
/* ========================================================================== */
#define ADDR_SIGNATURE (EEPROM_START_ADDR)
#define ADDR_DATA_BASE (EEPROM_START_ADDR + 1)

#define VALID_SIGNATURE (0xAB) /* для фіксації чистої пам'яті eeprom */

/* Заводські налаштування (Дефолти) */
static const int16_t DEFAULT_VALUES[PARAM_COUNT] = {
    250,  /* TARGET: 25.0 °C */
    0,    /* P0: 0 (Heat) */
    20,   /* P1: 2.0 °C гістерезис */
    800,  /* P2: 80.0 °C Максимум */
    -200, /* P3: -20.0 °C Мінімум */
    0,    /* P4: 0.0 °C Калібрування */
    0,    /* P5: 0 хвилин затримка */
    850   /* P6: 85.0 °C Аварія */
};

/* ========================================================================== */
/* ОПЕРАТИВНИЙ КЕШ (Пам'ять RAM)                                              */
/* ========================================================================== */
/* Копію налаштувань, щоб не читати EEPROM щосекунди */
static int16_t cache[PARAM_COUNT];

/* ========================================================================== */
/* ПРИВАТНІ ФУНКЦІЇ                                                           */
/* ========================================================================== */

/* Фізично пише 16-бітне число у 2 байти EEPROM */
static void SaveParamToEEPROM(param_id_t param, int16_t value) 
{
    /* Кожен параметр займає 2 байти, тому множимо індекс на 2 */
    uint16_t addr = ADDR_DATA_BASE + ((uint16_t)param * 2);
    
    uint8_t high_byte = (uint8_t)(value >> 8);
    uint8_t low_byte  = (uint8_t)(value & 0xFF);
    
    EEPROM_WriteByte(addr, high_byte);
    EEPROM_WriteByte(addr + 1, low_byte);
}

/* Фізично читає 16-бітне число з 2 байтів EEPROM */
static int16_t ReadParamFromEEPROM(param_id_t param) 
{
    uint16_t addr = ADDR_DATA_BASE + ((uint16_t)param * 2);
    
    uint8_t high_byte = EEPROM_ReadByte(addr);
    uint8_t low_byte  = EEPROM_ReadByte(addr + 1);
    
    return (int16_t)(((uint16_t)high_byte << 8) | low_byte);
}

/* ========================================================================== */
/* ПУБЛІЧНІ ФУНКЦІЇ (Бізнес-логіка)                                           */
/* ========================================================================== */

void Settings_Init(void) 
{
    uint8_t i;
    uint8_t sig = EEPROM_ReadByte(ADDR_SIGNATURE);
    
    if (sig == VALID_SIGNATURE) 
    {
        /* Нормальний старт: всі дані з EEPROM у Кеш */
        for (i = 0; i < (uint8_t)PARAM_COUNT; i++) 
        {
            cache[i] = ReadParamFromEEPROM((param_id_t)i);
        }
    } 
    else 
    {
        /* Перше увімкнення мікроконтролера: пам'ять порожня або пошкоджена */
        Settings_ResetToDefault();
        EEPROM_WriteByte(ADDR_SIGNATURE, VALID_SIGNATURE);
    }
}

int16_t Settings_Get(param_id_t param) 
{
    if (param >= PARAM_COUNT) return 0;
    return cache[param]; /* Миттєва віддача з RAM */
}

void Settings_Set(param_id_t param, int16_t value) 
{
    if (param >= PARAM_COUNT) return;
    
    /* Оновлюємо кеш */
    cache[param] = value;
    
    /* Зберігаємо фізично в незалежну пам'ять */
    SaveParamToEEPROM(param, value);
}

void Settings_ResetToDefault(void) 
{
    uint8_t i;
    for (i = 0; i < (uint8_t)PARAM_COUNT; i++) 
    {
        Settings_Set((param_id_t)i, DEFAULT_VALUES[i]);
    }
}