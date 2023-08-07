#include "ds3231.h"
#define I2C_100KHZ 

void DS3231_Set_Date_Time(uint8_t dia, uint8_t mes, uint8_t ano, uint8_t dw, uint8_t horas, uint8_t minutos, uint8_t segundos)
{
    segundos &= 0x7F;
    horas &= 0x3F;
    I2C_Master_Start();
    I2C_Master_Write(DS3231_ADDRESS);
    I2C_Master_Write(0x00);
    I2C_Master_Write(DS3231_Bin_Bcd(segundos));
    I2C_Master_Write(DS3231_Bin_Bcd(minutos));
    I2C_Master_Write(DS3231_Bin_Bcd(horas));
    I2C_Master_Write(DS3231_Bin_Bcd(dw));
    I2C_Master_Write(DS3231_Bin_Bcd(dia));
    I2C_Master_Write(DS3231_Bin_Bcd(mes));
    I2C_Master_Write(DS3231_Bin_Bcd(ano));
    I2C_Master_Stop();
}

void DS3231_Get_Date(uint8_t *dia, uint8_t *mes, uint8_t *ano, uint8_t *dow)
{
    *dow = DS3231_Bcd_Bin(DS3231_Read(DS3231_DAY) & 0x7F);
    *dia = DS3231_Bcd_Bin(DS3231_Read(DS3231_DATE) & 0x3F);
    *mes = DS3231_Bcd_Bin(DS3231_Read(DS3231_MONTH) & 0x1F);
    *ano = DS3231_Bcd_Bin(DS3231_Read(DS3231_YEAR));
}

void DS3231_Get_Time(uint8_t *horas, uint8_t *minutos, uint8_t *segundos)
{
    *segundos = DS3231_Bcd_Bin(DS3231_Read(DS3231_SEC) & 0x7F);
    *minutos = DS3231_Bcd_Bin(DS3231_Read(DS3231_MIN) & 0x7F);
    *horas = DS3231_Bcd_Bin(DS3231_Read(DS3231_HOUR) & 0x3F);
}

void DS3231_Get_DayOfWeek(char* str)
{
    uint8_t ldia;
    uint8_t lmes;
    uint8_t lano;
    uint8_t ldow;
    DS3231_Get_Date(&ldia, &lmes, &lano, &ldow);
    sprintf(str, "%s", dw[ldow]);
}

uint8_t DS3231_Read(uint8_t reg)
{
    uint8_t data = 0;
    I2C_Master_Start();
    I2C_Master_Write(DS3231_ADDRESS);
    I2C_Master_Write(reg);
    I2C_Master_RepeatedStart();
    I2C_Master_Write(DS3231_ADDRESS | 0x01);
    //data = I2C_Master_Read(I2C_100KHZ);
    I2C_Nack();
    I2C_Master_Stop();
    return data;
}

uint8_t DS3231_Bin_Bcd(uint8_t binary_value)
{
    uint8_t temp;
    uint8_t retval;
    temp = binary_value;
    retval = 0;
    while(1)
    {
        if(temp >= 10){
            temp -= 10;
            retval += 0x10;
        }else{
            retval += temp;
            break;
        }
    }
    return(retval);
}

uint8_t DS3231_Bcd_Bin(uint8_t bcd_value)
{
    uint8_t temp;
    temp = bcd_value;
    temp >>= 1;
    temp &= 0x78;
    return(temp + (temp >> 2) + (bcd_value & 0x0f));
}