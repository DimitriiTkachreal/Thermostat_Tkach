/**
 * @file    tm1637.c
 * @brief   Реалізація протоколу та бізнес-логіки TM1637
 */

#include "tm1637.h"

/* ========================================================================== */
/* НИЗЬКОРІВНЕВИЙ ТРАНСПОРТ (Bit-Banging)                                     */
/* ========================================================================== */

static void TM1637_Start(void) 
{
    CLK_HIGH();
    DIO_HIGH();
    DELAY_10US();
    DIO_LOW(); 
    DELAY_10US();
}

static void TM1637_Stop(void) 
{
    CLK_LOW();
    DELAY_10US();
    DIO_LOW();
    DELAY_10US();
    CLK_HIGH();
    DELAY_10US();
    DIO_HIGH(); 
    DELAY_10US();
}

static void TM1637_WriteByte(uint8_t data) 
{
    uint8_t i;
    for (i = 0; i < 8; i++) 
    {
        CLK_LOW();
        DELAY_10US();
        
        if (data & 0x01) { DIO_HIGH(); } 
        else             { DIO_LOW();  }
        DELAY_10US();
        
        CLK_HIGH();
        DELAY_10US();
        
        data = data >> 1; 
    }
    
    /* 9-й такт (ACK) */
    CLK_LOW();
    DIO_HIGH(); /* Відпускаємо лінію для підтвердження від дисплея */
    DELAY_10US();
    CLK_HIGH(); 
    DELAY_10US();
    CLK_LOW();  
    DELAY_10US();
}

/* Оптимізація (DRY): Відправка одиночної команди */
static void TM1637_SendCommand(uint8_t cmd) 
{
    TM1637_Start();
    TM1637_WriteByte(cmd);
    TM1637_Stop();
}

/* ========================================================================== */
/* БЛОК 3: БІЗНЕС-ЛОГІКА ТM1637 (Тепер лише базовий вивід)                */
/* ========================================================================== */

void TM1637_ShowRaw(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) 
{
    /* Відправка в дисплей */
    TM1637_SendCommand(0x40); /* Режим автоінкременту адреси */

    TM1637_Start();
    TM1637_WriteByte(0xC0);   /* Стартова адреса GRID1 */
    TM1637_WriteByte(d0);
    TM1637_WriteByte(d1);
    TM1637_WriteByte(d2);
    TM1637_WriteByte(d3); 
    TM1637_Stop();

    TM1637_SendCommand(0x8F); /* Увімкнути дисплей (макс. яскравість) */
}