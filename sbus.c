#include "stm32l476xx.h"
#include "sbus.h"
#include "stdio.h"
#include "string.h"

// Functie die alle variabelen uit de struct juist zet om te kunnen starten.
void SBUSInit(SBUS* sbus)
{
	// Hulpvariabele.
	uint8_t i = 0;
	
	for(i = 0; i < SBUS_NUMBER_OF_BYTES; i++)
		sbus->rawData[i] = 0;
	sbus->byteCounter = 0;
	sbus->timeOutCounter = 0;
	sbus->newFrameConverted = false;
	
	sbus->header = 0;
	for(i = 0; i < SBUS_NUMBER_OF_CHANNELS; i++)
		sbus->channel[i] = 0;
	sbus->channel17 = false;
	sbus->channel18 = false;
	sbus->frameLost = false;
	sbus->failSafe = false;
	sbus->footer = 0;
}


// Deze functie verwerkt de 25 ontvangen bytes, die via de SBUS binnenkwamen op een UART en zet ze om naar 'kanaalwaardes'.
// Let op, de SBUS is een UART met volgende eigenschappen: 8 databits, even pariteit, 2 stopbits, logisch geïnverteerd en op 100k baud.
// Deze functie moet opgeroepen worden na ieder 25 ontvangen bytes van de SBUS.
// De functie neemt een pointer naar een SBUS-struct mee.
// Geeft true terug als header en footer juist staan, anders false.
bool SBUSConvert(SBUS* sbus)
{
	// Tijdelijke variabele om de 16-bits die binnengeklokt zijn, bij te houden.
	uint16_t sbusChannelUnsignedTemp = 0;
	
	// Header en footer controleren. Indien ok, omzetting maken.
	if((sbus->rawData[0] == SBUS_HEADER_VALUE) && (sbus->rawData[SBUS_NUMBER_OF_BYTES - 1] == SBUS_FOOTER_VALUE))
	{	
		// De omzetting kan eventueel visueel ondersteund worden met een voorstelling van de bits in Excel.
		// Kanaal 1 berekenen.
		// Merk op: som van 'de twee' schuifoperaties is steeds 8. 
		// Een 11-bit masker = 0x07FF.
		sbusChannelUnsignedTemp = ((sbus->rawData[2] << 8) + sbus->rawData[1]) & 0x07FF;
		sbus->channel[0] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;

		// Kanaal 2 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[3] << 5) + (sbus->rawData[2] >> 3)) & 0x07FF;
		sbus->channel[1] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;

		// Kanaal 3 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[5] << 10) + (sbus->rawData[4] << 2) + (sbus->rawData[3] >> 6)) & 0x07FF;
		sbus->channel[2] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;

		// Kanaal 4 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[6] << 7) + (sbus->rawData[5] >> 1)) & 0x07FF;
		sbus->channel[3] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;

		// Kanaal 5 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[7] << 4) + (sbus->rawData[6] >> 4)) & 0x07FF;
		sbus->channel[4] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;

		// Kanaal 6 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[9] << 9) + (sbus->rawData[8] << 1) + (sbus->rawData[7] >> 7)) & 0x07FF;
		sbus->channel[5] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 7 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[10] << 6) + (sbus->rawData[9] >> 2)) & 0x07FF;
		sbus->channel[6] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 8 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[11] << 3) + (sbus->rawData[10] >> 5)) & 0x07FF;
		sbus->channel[7] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 9 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[13] << 8) + (sbus->rawData[12])) & 0x07FF;
		sbus->channel[8] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 10 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[14] << 5) + (sbus->rawData[13] >> 3)) & 0x07FF;
		sbus->channel[9] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 11 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[16] << 10) + (sbus->rawData[15] << 2) + (sbus->rawData[14] >> 6)) & 0x07FF;
		sbus->channel[10] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 12 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[17] << 7) + (sbus->rawData[16] >> 1)) & 0x07FF;
		sbus->channel[11] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 13 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[18] << 4) + (sbus->rawData[17] >> 4)) & 0x07FF;
		sbus->channel[12] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 14 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[20] << 9) + (sbus->rawData[19] << 1) + (sbus->rawData[18] >> 7)) & 0x07FF;
		sbus->channel[13] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 15 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[21] << 6) + (sbus->rawData[20] >> 2)) & 0x07FF;
		sbus->channel[14] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// Kanaal 16 berekenen.
		sbusChannelUnsignedTemp = ((sbus->rawData[22] << 3) + (sbus->rawData[21] >> 5)) & 0x07FF;
		sbus->channel[15] = (int16_t)sbusChannelUnsignedTemp - SBUS_MIDPOINT;
		
		// OPM: byte 23 wordt niet bij elke ontvanger ondersteund. Dikwijls maar 16 kanalen beschikbaar.
		// Kanaal 17 bekijken.
		if((sbus->rawData[23] & SBUS_CH17_BIT) == SBUS_CH17_BIT)
			sbus->channel17 = true;
		else
			sbus->channel17 = false;
		
		// Kanaal 18 bekijken.
		if((sbus->rawData[23] & SBUS_CH18_BIT) == SBUS_CH18_BIT)
			sbus->channel18 = true;
		else
			sbus->channel18 = false;
		
		// Frame lost bekijken.
		if((sbus->rawData[23] & SBUS_FRAME_LOST_BIT) == SBUS_FRAME_LOST_BIT)
			sbus->frameLost = true;
		else
			sbus->frameLost = false;
		
		// Fail safe bekijken.
		if((sbus->rawData[23] & SBUS_FAIL_SAFE_BIT) == SBUS_FAIL_SAFE_BIT)
			sbus->failSafe = true;
		else
			sbus->failSafe = false;
		
		return true;
	}
	else
	{		
		return false;
	}
}

// Zet de SBUS-struct om naar JSON-data.
char* SBUSToJSON(SBUS* sbus)
{
	// Variabelen. Eén daarvan is static, omdat die ook buiten de functie gebruikt moet worden.
	static char text[SBUS_MAX_JSON_LENGTH];
	char temp[SBUS_MAX_JSON_LENGTH];
	uint8_t i = 0;
	
	// JSON-pakket opbouwen.
	sprintf(text, "{\"channel\":[");
	for(i = 0; i < SBUS_NUMBER_OF_CHANNELS; i++)
	{
		sprintf(temp, "%d", sbus->channel[i]);
		strcat(text, temp);
		
		if(i < SBUS_NUMBER_OF_CHANNELS - 1)
			strcat(text, ",");
	}
	strcat(text, "],\"channel17\":");
	sprintf(temp, "%d", sbus->channel17);
	strcat(text, temp);
	strcat(text, ",\"channel18\":");
	sprintf(temp, "%d", sbus->channel18);
	strcat(text, temp);
	strcat(text, ",\"frameLost\":");
	sprintf(temp, "%d", sbus->frameLost);
	strcat(text, temp);
	strcat(text, ",\"failSafe\":");
	sprintf(temp, "%d", sbus->failSafe);
	strcat(text, temp);
	strcat(text, "}");
	
	return text;
}


// Voorbeeldgebruik in de main.c.
///* USER CODE BEGIN 2 */
//// Struct initialiseren.
//SBUSInit(&sbus);	

// ...

//// Receiver Timeout instellen op UART1 (SBUS UART). Deze timeout zorgt voor de synchronisatie.
//// OPM: code lijkt goed te werken met 150 (of meer) als timeout, maar niet stabiel met bijvoorbeeld 48...
//// In CMSIS werkte 48 wel stabiel...
//// Aangezien we op 100 000 baud werken, komt het neer op 150 x 10µs inactiviteit op de bus (1,5ms).
//HAL_UART_ReceiverTimeout_Config(&huart1, 150);
//HAL_UART_EnableReceiverTimeout(&huart1);

//// Ontvangst van 25 bytes via DMA starten.
//HAL_UART_Receive_DMA(&huart1, (uint8_t*)sbus.rawData, SBUS_NUMBER_OF_BYTES);	
///* USER CODE END 2 */


	
///* USER CODE BEGIN 4 */
//// Code hieronder wordt opgeroepen als alle data van HAL_UART_Receive_DMA(() ontvangen is...
//// Deze functie wordt opgeroepen vanuit HAL_UART_IRQHandler().
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
//{
//	// Is het data van UART1?
//	if(UartHandle->Instance == USART1)
//	{		
//		// In functie van scoopmetingen, LED8 omkeren.
//		HAL_GPIO_TogglePin(LED8_GPIO_Port, LED8_Pin);
//		
//		// LED1 inschakelen (i.f.v. tijdsregistratie met scoop).
//		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
//		
//		// SBUS-data proberen omzetten naar kanaalwaardes.
//		if(SBUSConvert(&sbus))
//			sbus.newFrameConverted = true;
//		
//		// LED1 uitschakelen (tijdsregistratie).
//		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
//	}
//}

//// Is er een Receiver Timeout geweest (hier behandeld als error)? Hersynchroniseer.
//// Let op: deze functie wordt enkel opgeroepen als 'UART1 Global Interrupt' aangevinkt staat in CubeMX!
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{	
//	if((huart->ErrorCode & HAL_UART_ERROR_RTO) == HAL_UART_ERROR_RTO)
//	{
//		// In functie van scoopmetingen, LED7 omkeren.
//		HAL_GPIO_TogglePin(LED7_GPIO_Port, LED7_Pin);
//		
//		// Ontvangst via DMA opnieuw starten. Want RXNEIE wordt automatisch uitgeschakeld.
//		HAL_UART_Receive_DMA(huart, (uint8_t*)sbus.rawData, SBUS_NUMBER_OF_BYTES);
//	}
//}
///* USER CODE END 4 */
