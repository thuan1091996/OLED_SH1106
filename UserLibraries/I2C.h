// *****************************************************************************
//
//------------------------- 0.Driver information -------------------------------
// This library is used to communicate between TM4C123 and I2C devices via I2C
// Created on: Nov, 6th  2019
// Author : TRAN MINH THUAN
// Driver associate with TM4C123, CCS version 9.x
// Hardware options:
//        |  SDA  |  SCL
// -------+-------+--------
//  I2C0  |  PB3  |  PB2
//  I2C1  |  PA7  |  PA6
//  I2C2  |  PE5  |  PE4
//  I2C3  |  PD1  |  PD0
//******************************************************************************

#ifndef USERLIBRARIES_I2C_H_
#define USERLIBRARIES_I2C_H_

#define I2C_0      0
#define I2C_1      1
#define I2C_2      2
#define I2C_3      3
#define I2C_MODULE I2C_3

#if I2C_MODULE == I2C_0
#define I2C_PERIPH          SYSCTL_PERIPH_I2C0
#define I2C_GPIO_PERIPH     SYSCTL_PERIPH_GPIOB
#define I2C_GPIO_PORT       GPIO_PORTB_BASE
#define I2C_PORT            I2C0_BASE
#define SCL_PIN             GPIO_PIN_2
#define SDA_PIN             GPIO_PIN_3
#define SCL_GPIO_CONFIG     GPIO_PB2_I2C0SCL
#define SDA_GPIO_CONFIG     GPIO_PB3_I2C0SDA


#elif I2C_MODULE == I2C_1
#define I2C_PERIPH          SYSCTL_PERIPH_I2C1
#define I2C_GPIO_PERIPH     SYSCTL_PERIPH_GPIOA
#define I2C_GPIO_PORT       GPIO_PORTA_BASE
#define I2C_PORT            I2C1_BASE
#define SCL_PIN             GPIO_PIN_6
#define SDA_PIN             GPIO_PIN_7
#define SCL_GPIO_CONFIG     GPIO_PA6_I2C1SCL
#define SDA_GPIO_CONFIG     GPIO_PA7_I2C1SDA

#elif I2C_MODULE == I2C_2
#define I2C_PERIPH          SYSCTL_PERIPH_I2C2
#define I2C_GPIO_PERIPH     SYSCTL_PERIPH_GPIOE
#define I2C_GPIO_PORT       GPIO_PORTE_BASE
#define I2C_PORT            I2C2_BASE
#define SCL_PIN             GPIO_PIN_4
#define SDA_PIN             GPIO_PIN_5
#define SCL_GPIO_CONFIG     GPIO_PE4_I2C2SCL
#define SDA_GPIO_CONFIG     GPIO_PE5_I2C2SDA

#elif I2C_MODULE == I2C_3
#define I2C_PERIPH          SYSCTL_PERIPH_I2C3
#define I2C_GPIO_PERIPH     SYSCTL_PERIPH_GPIOD
#define I2C_GPIO_PORT       GPIO_PORTD_BASE
#define I2C_PORT            I2C3_BASE
#define SCL_PIN             GPIO_PIN_0
#define SDA_PIN             GPIO_PIN_1
#define SCL_GPIO_CONFIG     GPIO_PD0_I2C3SCL
#define SDA_GPIO_CONFIG     GPIO_PD1_I2C3SDA
#endif /*# End of if I2C_MODULE*/

void        I2C_Init(bool b_FastMode);
uint32_t    I2C_Write_Byte(uint8_t ui8Slave_Addr, uint8_t ui8Data);
uint32_t    I2C_Write_Bytes(uint8_t ui8Slave_Addr, uint8_t *ptData, uint16_t ui16NumbByte);
uint8_t     I2C_Read_Byte(uint8_t ui8SlaveAddr, uint8_t ui8Reg_Addr);


#endif /* USERLIBRARIES_I2C_H_ */
