/**
 * @file    eeprom.h
 * @brief   Низькорівневий драйвер для роботи з Data EEPROM.
 * @details Забезпечує атомарне читання та безпечний запис із 
 * використанням апаратного розблокування MASS.
 */

#ifndef EEPROM_H
#define EEPROM_H

#include "board_config.h" 


/**
 * @brief Читає один байт з енергонезалежної пам'яті.
 * @param address Фізична адреса (наприклад, 0x4000)
 * @return Прочитаний байт
 */
uint8_t EEPROM_ReadByte(uint16_t address);

/**
 * @brief Безпечно записує один байт у пам'ять.
 * @details Автоматично відкриває сейф (MASS), записує дані 
 * і миттєво закриває сейф для захисту від збоїв.
 * @param address Фізична адреса для запису
 * @param data Байт даних
 */
void EEPROM_WriteByte(uint16_t address, uint8_t data);

#endif /* EEPROM_H */