#include "stm32l4xx_hal.h"

#ifndef MPU6500_DEFINED
	#define MPU6500_DEFINED
	
	#define SLAVE_ADDRESS_GYRO_ACC 0x68													// Slave address van de MPU6500 (7-bit gedeelte).
	#define SENSITIVITY_SCALE_FACTOR_GYRO	16.384f								// LSb/(°/s)
	#define SENSITIVITY_SCALE_FACTOR_ACCELEROMETER 16384.0f			// LSb/g	
	#define TEMP_OFFSET 0.0f
	#define	TEMP_SENSITIVITY 333.87f	
	
	// Adressen van de registers (die standaard de read/write bit op nul hebben = schrijven).
	#define CONFIG 				0x1A
	#define GYRO_CONFIG		0x1B
	#define ACCEL_CONFIG	0x1C
	#define ACCEL_CONFIG2	0x1D
	#define ACCEL_XOUT_H 	0x3B
	#define TEMP_OUT_H 		0x41
	#define GYRO_XOUT_H 	0x43
	#define USER_CTRL			0x6A
	#define PWR_MGMT_1		0x6B
	#define WHO_AM_I			0x75
	#define INT_PIN_CFG 	0x37
	
	typedef struct{
		float gyroX;
		float gyroY;
		float gyroZ;
		float accX;
		float accY;
		float accZ;
		float temp;
	} Mpu6500;
	
	void InitMpu6500(I2C_HandleTypeDef* hi2c);
	uint8_t WhoAmIMpu6500(I2C_HandleTypeDef* hi2c);
	Mpu6500* ReadMpu6500(I2C_HandleTypeDef* hi2c);
#endif
