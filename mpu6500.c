// Uitlezen van een MPU6500 via I²C (Sparkfun breakout board) (7-bit adres = 0x68).
//
// Dit breakout board bevat reeds 10kOhm pull-up weerstanden naar de 3V3.
//
// OPM: Alle registers van de MPU6500 staan na reset op 0x00, behalve Who Am I en het Power management register 
// (adres 0x6B = 107 staat op 0x01 = Auto selects the best available clock source – PLL if ready, else use the Internal oscillator).

#include "mpu6500.h"
#include "main.h"

// Globale variabele die later via 'extern' opgevraagd kan worden in andere bestanden.
Mpu6500* sensorData;

// Functie om de MPU-6500 te initialiseren.
void InitMpu6500(I2C_HandleTypeDef* hi2c)
{
	uint32_t waitSteps = 0;
	uint8_t data[2];	
	
	// MPU Resetten.
	data[0] = PWR_MGMT_1;
	data[1] = 0x80;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 2, 100);	
	
	// Wachten tot heropstart...
	for(waitSteps = 0; waitSteps < 500000 ; waitSteps++)
		__NOP();	
	
	// Range Gyro instellen op 2000°/s.
	data[0] = GYRO_CONFIG;
	data[1] = 0x18;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 2, 100);
	
	// Digital filter van de accelerometer op 41Hz instellen.
	data[0] = ACCEL_CONFIG2;
	data[1] = 0x03;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 2, 100);
	
	// Digital filter van de gyroscoop op 41Hz instellen.
	data[0] = CONFIG;
	data[1] = 0x03;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 2, 100);
	
	// I²C Bypass enable modus.
	data[0] = INT_PIN_CFG;
	data[1] = 0x02;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 2, 100);
	
	// Standaard staat de accelerometer op +-2g.
}

// Who am I opvragen...
uint8_t WhoAmIMpu6500(I2C_HandleTypeDef* hi2c)
{	
	uint8_t whoAmI[1];
	
	// Who am I register.
	whoAmI[0] = WHO_AM_I;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), whoAmI, 1, 100);
	
	// 1 byte aan data uitlezen via blokkerende functie.
	HAL_I2C_Master_Receive(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), whoAmI, 1, 100);
	
	return whoAmI[0];
}

// Sensorwaardes uitlezen.
Mpu6500* ReadMpu6500(I2C_HandleTypeDef* hi2c)
{	
	static Mpu6500 sensordata;
	uint8_t data[14];
	
	// Register 0x3B: Acc X High byte.
	data[0] = ACCEL_XOUT_H;
	HAL_I2C_Master_Transmit(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 1, 100);
	
	// 14 bytes aan IMU-data lezen.
	HAL_I2C_Master_Receive(hi2c, (SLAVE_ADDRESS_GYRO_ACC << 1), data, 14, 100);
	
	// Waardes uitrekenen accelerometer, gyroscoop en temperatuursensor.
	sensordata.accX = (float)((int16_t)((data[0] << 8) | data[1]))/SENSITIVITY_SCALE_FACTOR_ACCELEROMETER;
	sensordata.accY = (float)((int16_t)((data[2] << 8) | data[3]))/SENSITIVITY_SCALE_FACTOR_ACCELEROMETER;
	sensordata.accZ = (float)((int16_t)((data[4] << 8) | data[5]))/SENSITIVITY_SCALE_FACTOR_ACCELEROMETER;
	sensordata.temp = ((float)((int16_t)((data[6] << 8) | data[7])) - TEMP_OFFSET)/TEMP_SENSITIVITY + 21.0f;
	sensordata.gyroX = (float)((int16_t)((data[8] << 8) | data[9]))/SENSITIVITY_SCALE_FACTOR_GYRO;
	sensordata.gyroY = (float)((int16_t)((data[10] << 8) | data[11]))/SENSITIVITY_SCALE_FACTOR_GYRO;
	sensordata.gyroZ = (float)((int16_t)((data[12] << 8) | data[13]))/SENSITIVITY_SCALE_FACTOR_GYRO;
	
	return &sensordata;
}
