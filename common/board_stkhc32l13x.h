
/******************************************************************************/
/** \file stkhc32l13x.h
 **
 ** stk board common define.
 ** @link SampleGroup Some description @endlink
 **
 **   - 2018-03-09  1.0  Lux First version.
 **
 ******************************************************************************/
#ifndef __BOARD_STKHC32L13X_H__
#define __BOARD_STKHC32L13X_H__

///< STK GPIO DEFINE
///< USER KEY
#define STK_USER_PORT       GpioPortA
#define STK_USER_PIN        GpioPin8


#define STK_USER_PIN1        GpioPin4

#define STK_USER_PIN2        GpioPin5

///< LED
//#define STK_LED_PORT        GpioPortC
//#define STK_LED_PIN         GpioPin13

///< LED

#define STK_LED_PIN1         GpioPin14
///< LED
#define STK_LED_PIN2         GpioPin15


///< XTH
#define SYSTEM_XTH          (32*1000*1000u)     ///< 32MHz

#define STK_XTHI_PORT       GpioPortD
#define STK_XTHI_PIN        GpioPin0
#define STK_XTHO_PORT       GpioPortD
#define STK_XTHO_PIN        GpioPin1

///< XTL
#define SYSTEM_XTL          (32768u)            ///< 32768Hz
#define STK_XTLI_PORT       GpioPortC
#define STK_XTLI_PIN        GpioPin14
#define STK_XTLO_PORT       GpioPortC
#define STK_XTLO_PIN        GpioPin15

///< LCD
#define STK_LCD_COM0_PORT   GpioPortA
#define STK_LCD_COM0_PIN    GpioPin9
#define STK_LCD_COM1_PORT   GpioPortA
#define STK_LCD_COM1_PIN    GpioPin10
#define STK_LCD_COM2_PORT   GpioPortA
#define STK_LCD_COM2_PIN    GpioPin11
#define STK_LCD_COM3_PORT   GpioPortA
#define STK_LCD_COM3_PIN    GpioPin12
#define STK_LCD_SEG0_PORT   GpioPortA
#define STK_LCD_SEG0_PIN    GpioPin8
#define STK_LCD_SEG1_PORT   GpioPortC
#define STK_LCD_SEG1_PIN    GpioPin9
#define STK_LCD_SEG2_PORT   GpioPortC
#define STK_LCD_SEG2_PIN    GpioPin8
#define STK_LCD_SEG3_PORT   GpioPortC
#define STK_LCD_SEG3_PIN    GpioPin7
#define STK_LCD_SEG4_PORT   GpioPortC
#define STK_LCD_SEG4_PIN    GpioPin6
#define STK_LCD_SEG5_PORT   GpioPortB
#define STK_LCD_SEG5_PIN    GpioPin15
#define STK_LCD_SEG6_PORT   GpioPortB
#define STK_LCD_SEG6_PIN    GpioPin14
#define STK_LCD_SEG7_PORT   GpioPortB
#define STK_LCD_SEG7_PIN    GpioPin13

///< I2C EEPROM
#define EVB_I2C0_EEPROM_SCL_PORT    GpioPortB
#define EVB_I2C0_EEPROM_SCL_PIN     GpioPin6
#define EVB_I2C0_EEPROM_SDA_PORT    GpioPortB
#define EVB_I2C0_EEPROM_SDA_PIN     GpioPin7

///< SPI0
#define EVB_SPI0_FLASH_CS_PORT      GpioPortE
#define EVB_SPI0_FLASH_CS_PIN       GpioPin12
#define EVB_SPI0_FLASH_SCK_PORT     GpioPortE
#define EVB_SPI0_FLASH_SCK_PIN      GpioPin13
#define EVB_SPI0_FLASH_MISO_PORT    GpioPortE
#define EVB_SPI0_FLASH_MISO_PIN     GpioPin14
#define EVB_SPI0_FLASH_MOSI_PORT    GpioPortE
#define EVB_SPI0_FLASH_MOSI_PIN     GpioPin15

#endif