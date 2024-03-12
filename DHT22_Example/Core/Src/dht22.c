/*
 * dht22.c
 *
 *  Created on: Mar 11, 2024
 *      Author: Michal Klebokowski
 */
#include "dht22.h"
#include "gpio.h"
#include "tim.h"

//
// -- Functions to port --
//

/* These functions need to be ported to for your specific platform */
/* Each functions role is descibed above */

/* Custom 1 us delay */
void DHT22_Delay(int us)
{
	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim11))<us);
}

/* GPIO pin's direction change */
void DHT22_SetPinAsOutput (DHT22_t *sensor)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = sensor->Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(sensor->Port, &GPIO_InitStruct);
}

void DHT22_SetPinAsInput (DHT22_t *sensor)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = sensor->Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(sensor->Port, &GPIO_InitStruct);
}

/* GPIO pin's set */
void DHT22_SetPin(DHT22_t *sensor, uint8_t PinState)
{
	HAL_GPIO_WritePin(sensor->Port, sensor->Pin, PinState);
}
/* GPIO pin's read */
uint8_t DHT22_ReadPin(DHT22_t *sensor)
{
	return HAL_GPIO_ReadPin (sensor->Port, sensor->Pin);
}

/* Timeouted wait for pin state change */
DHT22_Error_t DHT22_WaitForPin(DHT22_t *sensor, uint8_t PinState)
{
	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while (!(DHT22_ReadPin(sensor) == PinState))
	{
		if(__HAL_TIM_GET_COUNTER(&htim11) >= (DHT22_TIMEOUT_MS * 1000))
		{
			/* Timeout ! */
			return DHT22_TIMEOUT;
		}
	}
	/* Pin went desired state */
	return DHT22_OK;
}


/* Init */
void DHT22_Init(DHT22_t *sensor, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	sensor->Port = GPIOx;
	sensor->Pin = GPIO_Pin;
}

//
// -- Communication --
//

/* Starting measurement */
DHT22_Error_t DHT22_SendStartSignal(DHT22_t *sensor)
{
	/* Send start signal (put line low for >1ms) */
	DHT22_SetPinAsOutput(sensor);
	DHT22_SetPin(sensor, 0);
	DHT22_Delay(1200);
	DHT22_SetPin(sensor, 1);

	/* Set pin as input and wait for response */
	DHT22_SetPinAsInput(sensor);
	DHT22_Delay(40);

	/* Check if sensor has put line low */
	if (DHT22_ReadPin(sensor) == 0)
	{
		DHT22_Delay(80);
		/* If the line is still low after 80us, response isn't correct */
		if (DHT22_ReadPin(sensor) == 0)
		{
			sensor->presence = 0;
			return DHT22_ERROR;
		}
	}
	/* Wait for it to put line high again */
	if(DHT22_WaitForPin(sensor, 0) == 1)
	{
		sensor->presence = 0;
		return DHT22_TIMEOUT;
	}

	sensor->presence = 1;
	DHT22_Delay(20);
	return DHT22_OK;
}


/* Reading one byte */
DHT22_Error_t DHT22_ReadByte(DHT22_t *sensor, uint8_t *byte)
{
	uint8_t i;

	/* Read every bit */
	for (i=0; i<8; i++)
	{
		/* Wait for line to go high */
		if(DHT22_WaitForPin(sensor, 1) == 1)
		{
			sensor->presence = 0;
			return DHT22_TIMEOUT;
		}

		/* Wait 40ms */
		DHT22_Delay(40);

		/* Check if line is low */
		if (DHT22_ReadPin(sensor) == 0)
		{
			/* Write 0 */
			*byte &= ~(1<<(7-i));
		}
		/* If line is high */
		else
		{
			/* Write 1 */
			*byte |= (1<<(7-i));
			/* Wait for line to go low again */
			if(DHT22_WaitForPin(sensor, 0) == 1)
			{
				sensor->presence = 0;
				return DHT22_TIMEOUT;
			}

		}

		/* Wait for the line to go high again */

		if(DHT22_WaitForPin(sensor, 1) == 1)
		{
			sensor->presence = 0;
			return DHT22_TIMEOUT;
		}
	}
	return DHT22_OK;
}

DHT22_Error_t DHT22_ChecksumVerify(DHT22_t *sensor, uint8_t receivedChecksum)
{
	uint32_t Sum;
	uint8_t calculatedChecksum;
	Sum = sensor->RH_byte1 + sensor->RH_byte2 + sensor->Temp_byte1 + sensor->Temp_byte2;
	calculatedChecksum = Sum & 0xFF;
	if(calculatedChecksum == receivedChecksum)
	{
		return DHT22_OK;
	}
	else
	{
		return DHT22_ERROR;
	}
}


/* Read all data */
DHT22_Error_t DHT22_ReadData(DHT22_t *sensor)
{
	/* Save initial presence value */
	uint8_t initial_presence = sensor->presence;

	if(DHT22_SendStartSignal(sensor) == DHT22_OK)
	{
		/* If sensor is back after failure, discard first measurement */
		if(initial_presence != sensor->presence)
		{
			return DHT22_OK;
		}
		uint8_t checksum;

		if(DHT22_ReadByte(sensor, &(sensor->RH_byte1)) != DHT22_OK) return DHT22_ERROR;
		if(DHT22_ReadByte(sensor, &(sensor->RH_byte2)) != DHT22_OK) return DHT22_ERROR;
		if(DHT22_ReadByte(sensor, &(sensor->Temp_byte1)) != DHT22_OK) return DHT22_ERROR;
		if(DHT22_ReadByte(sensor, &(sensor->Temp_byte2)) != DHT22_OK) return DHT22_ERROR;
		if(DHT22_ReadByte(sensor, &checksum) != DHT22_OK) return DHT22_ERROR;
		if(DHT22_ChecksumVerify(sensor, checksum) != DHT22_OK) return DHT22_ERROR;


		sensor->Humidity_raw = ((sensor->RH_byte1<<8)|sensor->RH_byte2);
		sensor->Temperature_raw = ((sensor->Temp_byte1<<8)|sensor->Temp_byte2);

		sensor->Temperature = (float) (sensor->Temperature_raw/10.0);
		sensor->Humidity = (float) (sensor->Humidity_raw/10.0);
		return DHT22_OK;

	}
	else
	{
		return DHT22_ERROR;
	}
}

//
// -- Getters --
//
float DHT22_GetTemperature(DHT22_t *sensor)
{
	return sensor->Temperature;
}

float DHT22_GetHumidity(DHT22_t *sensor)
{
	return sensor->Humidity;
}

uint8_t DHT22_GetPresence(DHT22_t *sensor)
{
	return sensor->presence;
}
