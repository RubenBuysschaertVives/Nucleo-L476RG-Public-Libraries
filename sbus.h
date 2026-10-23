#include "stm32l476xx.h"
#include "stdbool.h"

#if !defined(SBUS_DEFINED)
	#define SBUS_DEFINED
	
	// Defines die de code leesbaarder en onderhoudsvriendelijker maken.
	#define SBUS_HEADER_VALUE 0x0F
	#define SBUS_FOOTER_VALUE 0x00
	#define SBUS_CH17_BIT 0x80
	#define SBUS_CH18_BIT 0x40
	#define SBUS_FRAME_LOST_BIT 0x20
	#define SBUS_FAIL_SAFE_BIT 0x10
	#define SBUS_NUMBER_OF_CHANNELS 16
	#define SBUS_NUMBER_OF_BYTES 25
	#define SBUS_TIME_OUT 3													// Na 3ms geen data ontvangen te hebben van de SBUS, reset de indexer.
	#define SBUS_MIDPOINT 992												// Zou in feite de helft van een signed 16-bit integer moeten zijn... 1023?
																									// Waardes liggen in principe tussen de 192 en 1792. Dus 992 zou het 'perfecte' midden moeten zijn.
																									// Zie: https://github.com/uzh-rpg/rpg_quadrotor_control/wiki/SBUS-Protocol.
																								
	#define SBUS_MAX_JSON_LENGTH	201								// Maximum aantal karakters dat verwacht worden op de JSON output.
													
	// Struct die alle data van een SBUS-frame samenhoudt.
	typedef struct{
		// Hulpvariabelen om data te kunnen ontvangen via een USART.
		volatile uint8_t rawData[SBUS_NUMBER_OF_BYTES];				// Een SBUS frame bezit 25 bytes aan data (voor 16 11 bit kanalen).
		volatile uint8_t byteCounter;													// Teller die aangeeft de hoeveelste byte van het SBUS frame nu binnenkomt.
		volatile uint8_t timeOutCounter;											// Teller die een time out op de SBUS kan bijhouden. Die kan dan weer gebruikt worden om te synchroniseren.
		volatile bool newFrameConverted;											// Als een nieuw SBUS-frame omgezet is, kan je dat hiermee laten weten. Het gebruik is optioneel. Wissen van de vlag moet door de eindgebruiker gebeuren.
		
		// Logische opbouw van een SBUS-frame.
		uint8_t header;																				// Header byte moet steeds 0x0F zijn.
		int16_t channel[SBUS_NUMBER_OF_CHANNELS];							// De omgezette (signed) kanaalwaardes. Let op: zero based!
		bool channel17;																				// Kanaal 17.
		bool channel18;																				// Kanaal 18.
		bool frameLost;																				// Frame lost.
		bool failSafe;																				// Fail safe actief.
		uint8_t footer;																				// Footer byte moet steeds 0 zijn.
	} SBUS;
	
	
	// Prototypes van functies.
	void SBUSInit(SBUS* sbus);
	bool SBUSConvert(SBUS* sbus);
	char* SBUSToJSON(SBUS* sbus);
#endif
