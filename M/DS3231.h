 
#ifndef DS3231_H
#define	DS3231_H

#include <xc.h>
#define _XTAL_FREQ 8000000

#include <stdint.h>
#include <stdio.h>
#include "i2c.h"

#define DS3231_ADDRESS  0xD0

#define DS3231_SEC      0x00
#define DS3231_MIN      0x01
#define DS3231_HOUR     0x02
#define DS3231_DAY      0x03
#define DS3231_DATE     0x04
#define DS3231_MONTH    0x05
#define DS3231_YEAR     0x06

extern char dw[7][11] = {"Domingo\0","Lunes\0","Martes\0","Miercoles\0","Jueves\0","Viernes\0","Sabado\0"};

void DS3231_Set_Date_Time(uint8_t dia, uint8_t mes, uint8_t ano, uint8_t dw, uint8_t horas, uint8_t minutos, uint8_t segundos);
void DS3231_Get_Date(uint8_t *dia, uint8_t *mes, uint8_t *ano, uint8_t *dow);
void DS3231_Get_Time(uint8_t *horas, uint8_t *minutos, uint8_t *segundos);
void DS3231_Get_DayOfWeek(char* str);
uint8_t DS3231_Read(uint8_t reg);
uint8_t DS3231_Bin_Bcd(uint8_t binary_value);
uint8_t DS3231_Bcd_Bin(uint8_t bcd_value);

#endif	/* XC_HEADER_TEMPLATE_H */

