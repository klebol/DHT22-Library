/*
 * hd44780.h
 *
 *  Created on: Feb 19, 2024
 *      Author: Michal Klebokowski
 */

#ifndef INC_HD44780_H_
#define INC_HD44780_H_

#include <stdint.h>

#define CORE_FREQUENCY_MHZ 84
#define LCD_ROWS 2
#define LCD_COLS 16

#define LCD_LINE1     0x00
#define LCD_LINE2     0x40
#define LCD_LINE3     0x14
#define LCD_LINE4     0x54

#define LCD_CLEAR         0x01
#define LCD_HOME          0x02
#define LCD_ENTRY_MODE       0x04
  #define LCD_EM_SHIFT_CURSOR     0x00
  #define LCD_EM_SHIFT_DISPLAY    0x01
  #define LCD_EM_LEFT           0x00
  #define LCD_EM_RIGHT        0x02
#define LCD_ONOFF         0x08
  #define LCD_DISP_ON         0x04
  #define LCD_CURSOR_ON       0x02
  #define LCD_CURSOR_OFF       0x00
  #define LCD_BLINK_ON       0x01
  #define LCD_BLINK_OFF        0x00
#define LCD_SHIFT         0x10
  #define LCD_SHIFT_DISP       0x08
  #define LCD_SHIFT_CURSOR     0x00
  #define LCD_SHIFT_RIGHT      0x04
  #define LCD_SHIFT_LEFT       0x00
#define LCD_FUNC          0x20
  #define LCD_8_BIT         0x10
  #define LCD_4_BIT         0x00
  #define LCD_TWO_LINE       0x08
  #define LCD_FONT_5x10        0x04
  #define LCD_FONT_5x7       0x00
#define LCD_SET_CGRAM        0x40
#define LCD_SET_DDRAM        0x80

/* Init LCD */
void LCD_Init(void);
/* Clear the display */
void LCD_Clear(void);
/* Set curosr position */
void LCD_Locate(uint8_t x, uint8_t y);
/* Reset cursor position */
void LCD_Home(void);
/* Write one character */
void LCD_WriteChar(char c);
/* Write string */
void LCD_WriteStr(char* str);
/* Write integer */
void LCD_WriteInt(int val);
/* Write HEX */
void LCD_WriteHex(int val);


#endif /* INC_HD44780_H_ */
