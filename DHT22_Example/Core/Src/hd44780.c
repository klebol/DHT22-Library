/*
 * hd44780.c
 *
 *  Created on: Feb 19, 2024
 *      Author: Michal Klebokowski
 */
#include "hd44780.h"
#include <stdlib.h>
#include <string.h>
#include "gpio.h"

/* Macros for setting RS, RW and E */
#define LCD_RS_LOW  HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET)
#define LCD_RS_HIGH HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET)

#define LCD_E_LOW   HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET)
#define LCD_E_HIGH  HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET)


/* 1us Delay implementation */
static void LCD_Delay(int us)
{
	for (int i = 0; i < CORE_FREQUENCY_MHZ * us; i++)
	{
	asm("nop");
	}
}

/* Sending half of data via 4 pin interface */
static void LCD_SendHalf(uint8_t data)
{
	/* Set Enable Pin high */
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
	LCD_Delay(5);
	/* Write half of data to data pins */
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, data & 0x01);
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, data & 0x02);
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, data & 0x04);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, data & 0x08);
	/* Clear E pin */
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin,  GPIO_PIN_RESET);
	LCD_Delay(5);
}

/* Send whole byte */
static void LCD_WriteByte(uint8_t data)
{
	LCD_SendHalf(data >> 4);
	LCD_SendHalf(data);
	LCD_Delay(60);
}

/* Send command */
static void LCD_WriteCmd(uint8_t cmd)
{
	LCD_RS_LOW;
	LCD_WriteByte(cmd);
}

static void LCD_WriteData(uint8_t data)
{
	LCD_RS_HIGH;
	LCD_WriteByte(data);
}

/* Initialization of GPIO pins */
static void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* E Pin initialize */
	GPIO_InitStruct.Pin = E_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(E_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);

	/* RW Pin initialize */
	GPIO_InitStruct.Pin = RS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RS_GPIO_Port, &GPIO_InitStruct);

	/* D4 Pin initialize */
	GPIO_InitStruct.Pin = D4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(D4_GPIO_Port, &GPIO_InitStruct);

	/* D5 Pin initialize */
	GPIO_InitStruct.Pin = D5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(D5_GPIO_Port, &GPIO_InitStruct);

	/* D6 Pin initialize */
	GPIO_InitStruct.Pin = D6_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(D6_GPIO_Port, &GPIO_InitStruct);

	/* D7 Pin initialize */
	GPIO_InitStruct.Pin = D7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(D7_GPIO_Port, &GPIO_InitStruct);
}

/* Init */
void LCD_Init(void)
{
	/* Init Data pins */
	LCD_GPIO_Init();
	HAL_Delay(15);

	/* Start sending commands */
	LCD_E_LOW;
	LCD_RS_LOW;

	/* Set 4 bit interface */
	LCD_SendHalf(0x03);
	HAL_Delay(4);
	LCD_Delay(100);
	LCD_SendHalf(0x03);
	LCD_Delay(100);
	LCD_SendHalf(0x03);
	LCD_Delay(100);
	LCD_SendHalf(0x02);
	LCD_Delay(100);

	/* 4 Bits, 2 rows, font 5x7 */
	LCD_WriteCmd(LCD_FUNC | LCD_4_BIT | LCD_TWO_LINE | LCD_FONT_5x7);
	/* Cursor off */
	LCD_WriteCmd(LCD_ONOFF | LCD_CURSOR_OFF);
	/* Display ON */
	LCD_WriteCmd(LCD_ONOFF | LCD_DISP_ON);
	/* Clear display */
	LCD_WriteCmd(LCD_CLEAR);
	HAL_Delay(5);
	/* Cursor shift right without display leters shift */
	LCD_WriteCmd(LCD_ENTRY_MODE | LCD_EM_SHIFT_CURSOR | LCD_EM_RIGHT);
}

/* Clear the display */
void LCD_Clear(void)
{
	LCD_WriteCmd(LCD_CLEAR);
}

/* Set curosr position */
void LCD_Locate(uint8_t x, uint8_t y)
{
	switch(y)
	{
		case 0: y = LCD_LINE1; break;

#if (LCD_ROWS>1)
		case 1: y = LCD_LINE2; break;
#endif
#if (LCD_ROWS>2)
		case 2: y = LCD_LINE3; break;
#endif
#if (LCD_ROWS>3)
		case 3: y = LCD_LINE4; break;
#endif
	}

	LCD_WriteCmd( (0x80 + y + x) );
}

/* Reset cursor position */
void LCD_Home(void)
{
	LCD_WriteCmd(LCD_CLEAR|LCD_HOME);
}

/* Write one character */
void LCD_WriteChar(char c)
{
	LCD_WriteData(c);
}

/* Write string */
void LCD_WriteStr(char* str)
{
	while(*str)
	{
		LCD_WriteChar(*str++);
	}
}


/* Write integer */
void LCD_WriteInt(int val)
{
	char Buffer[20];
	itoa(val, Buffer, 10);
	LCD_WriteStr(Buffer);
}

/* Write HEX */
void LCD_WriteHex(int val)
{
  char Buffer[15];
  itoa(val, Buffer, 16);
  LCD_WriteStr(Buffer);
}
