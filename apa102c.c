#include "stm32l4xx_hal.h"
#include "apa102c.h"

// Verstuur de data over SPI1. Let op: zorg dat SPI1 reeds geïnitialiseerd is.
void UpdateAPA102CLeds(SPI_HandleTypeDef* hspi, APA102C led[])
{
	uint8_t index = 0;
	uint8_t temp[] = {0, 0, 0, 0};
	
	HAL_SPI_Transmit(hspi, temp, sizeof(temp), 100);		// Start frame.
	
	for(index = 0; index < NUMBER_OF_APA102C_LEDS; index++)
	{	
		temp[0] = led[index].brightness | 0xE0;					// 5-bit intensiteit.
		temp[1] = led[index].blue;											// Blauw.
		temp[2] = led[index].green;											// Groen.
		temp[3] = led[index].red;												// Rood.
		
		HAL_SPI_Transmit(hspi, temp, sizeof(temp), 100);
	}
	
	temp[0] = 255;
	temp[1] = 255;
	temp[2] = 255;
	temp[3] = 255;
	HAL_SPI_Transmit(hspi, temp, sizeof(temp), 100);		// End frame.
}
