/*
 * dht22.h
 *
 *  Created on: Mar 11, 2024
 *      Author: Michal Klebokowski
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "gpio.h"

#define DHT22_TIMEOUT_MS 65

typedef struct
{
	/* Specific HAL pin and port info */
	GPIO_TypeDef *Port;
	uint16_t Pin;
	/* presence flag */
	uint8_t presence;
	/* received data */
	uint8_t RH_byte1;
	uint8_t RH_byte2;
	uint8_t Temp_byte1;
	uint8_t Temp_byte2;
	/* raw shifted data */
	uint16_t Humidity_raw;
	uint16_t Temperature_raw;
	/* calculated data */
	float Humidity;
	float Temperature;
}DHT22_t;

typedef enum
{
	DHT22_OK,
	DHT22_TIMEOUT,
	DHT22_ERROR
}DHT22_Error_t;

/* Init */
void DHT22_Init(DHT22_t *sensor, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
/* Query data */
DHT22_Error_t DHT22_ReadData(DHT22_t *sensor);
/* Getters */
float DHT22_GetTemperature(DHT22_t *sensor);
float DHT22_GetHumidity(DHT22_t *sensor);
uint8_t DHT22_GetPresence(DHT22_t *sensor);


#endif /* INC_DHT22_H_ */


